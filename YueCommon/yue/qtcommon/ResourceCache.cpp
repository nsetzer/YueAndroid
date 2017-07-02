

#include "yue/qtcommon/ResourceCache.h"

/**

TODO: resource number of subscribers should be updated, even after a successful
load. never evict items if it has an active subscriber.

 The Resource Cache is a cache of resources that take a non-trivial time to load.

 The basic strategy to use the the resource cache is to make a request
 with a unique id and additional parameters to aid in loading the resource.
 If the resource is no longer required, revoke the request. When the
 resource is ready, the subscriber will be notified. If the resource
 is already ready, it will be returned at the time the request is made.

 Subclass ResourceCacheThread and implement loadResource. the function
 will be run inside a thread and should return resource related
 to the given rid.

 Subclass ResourceCache and implement newWorkerThread() to return your
 custom workers.

 Use RAII semantics in your subscriber class to make requests and revoke
 a resource.

 TODO: provide an API in the cache for `bool validate(rid,userData, resource)`
 return true if the resource matches the rid and userData. this function
 is virtual and returns true by default.

 */
namespace yue {
namespace qtcommon {


void ResourceCacheThread::run()
{
    m_pCache->m_mutex.lock();
    while (m_pCache->m_alive) {
        if (m_pCache->m_requests.size()==0) {
            //qDebug() << "sleep worker";
            m_pCache->m_cond.wait(&m_pCache->m_mutex);
            //qDebug() << "wake worker";
        }
        if (!m_pCache->m_alive) {
            m_pCache->m_mutex.unlock();
            break;
        }

        // acquire a resource
        // find the best resource to process now
        ResourceRequestItem* res = m_pCache->acquire();

        if (res==nullptr) {
            continue;
        }

        ResourceCache::rid_t rid = res->m_rid;
        QVariant userData = res->m_userData;
        QVariant resource;
        // process the resource request
        m_pCache->m_mutex.unlock();
        // the danger zone

        bool success = run_main(rid,userData,&resource);

        m_pCache->m_mutex.lock();

        // the res could have been deleted while processing the request.
        // check the resource map to see if the resource is still needed
        // and then reaquire the resource pointer
        if (m_pCache->m_mapResource.count(rid) > 0) {
            res = *m_pCache->m_mapResource.find(rid);
            res->m_resource = resource;

        } else {
            success = false;
            res = nullptr;
            //qDebug() << "far flung fates feebly followed";
        }

        if (success) {
            res->setState(ResourceRequestItem::State::Complete);
            //qDebug() << "updating lrucache";
            m_pCache->m_lruqueue.enqueue(res->m_rid);
        } else if (res != nullptr) {
            delete res;
        }
    }

}

bool ResourceCacheThread::run_main(ResourceCache::rid_t rid, QVariant userData, QVariant* resource)
{
    bool success = true;
    try {

        //do the heavy work;
        *resource = loadResource(rid, userData);
        //notify anyone who cares that the work is ready;
        m_pCache->doNotify(rid,*resource);

    } catch (std::exception& e) {
        qWarning() << "Unhandled Exception: " << e.what();
        success = false;
    } catch (...) {
        qWarning() << "unhandled error";
        success = false;
    }

    return success;
}

void ResourceCacheThread::join()
{
    //qDebug() << "join worker";
    wait();

}

ResourceCache::ResourceCache(int numThreads, size_t cacheSize, QObject* parent/* = nullptr */)
    : QObject(parent)
    , m_alive(true)
    , m_numThreads(numThreads)
    , m_cacheSize(cacheSize)
{
    //qDebug() << "create cache";


}

void ResourceCache::init()
{
    if (m_workerThreads.size()==0) {
        for (int i=0; i < m_numThreads; i++) {
            //qDebug() << "create worker" << i;
            ResourceCacheThread* thread = newWorkerThread();
            thread->start();
            m_workerThreads.push_back(thread);
        }
    }
}

ResourceCache::~ResourceCache()
{

    {
        QMutexLocker lk(&m_mutex);
        //qDebug() << "set not alive and wake all";
        m_alive = false;
        m_cond.wakeAll();
    }

    for (ResourceCacheThread* thread : m_workerThreads) {
        thread->join();
        delete thread;
    }

    //qDebug() << "destroy cache";
}

QVariant ResourceCache::request(ResourceCache::rid_t rid, QVariant data/*=QVariant()*/)
{
    ResourceRequestItem* res;
    QMutexLocker lk(&m_mutex);
    QVariant variant;

    init(); // on first call, init the cache threads

    if (m_mapResource.count(rid) > 0) {

        // if the cache contains the item, return it immediately
        res = *m_mapResource.find(rid);
        // always increment, so that the resouce has an
        // idea of the number of subscribers
        res->increment();

        // if the request is compelete, return the resource
        if (res->getState() == ResourceRequestItem::State::Complete) {

            if (valid(rid,data,res->m_resource)) {
                // update the lru and return the current item
                //m_cache.update(res);
                m_lruqueue.removeOne(res->m_rid);
                m_lruqueue.enqueue(res->m_rid);
                variant = res->m_resource;
            } else {
                // tODO remove lru
                //m_lrucache.remove(res);
                m_lruqueue.removeOne(res->m_rid);
                m_mapResource.erase(m_mapResource.find(res->m_rid));
                createRequest(rid, data);
            }
        } else /*Queued*/ {
            // update the request.
            res->m_userData = data;
        }
    } else {
        createRequest(rid, data);
    }

    while (m_lruqueue.size() > m_cacheSize) {
        ResourceCache::rid_t tmp = m_lruqueue.dequeue();
        auto it = m_mapResource.find(tmp);
        ResourceRequestItem* res_tmp = *it;
        m_mapResource.erase(it);
        delete res_tmp;
    }

    return variant;
}

void ResourceCache::revoke(ResourceCache::rid_t rid)
{
    QMutexLocker lk(&m_mutex);

    if (m_mapResource.count(rid) > 0) {
        ResourceRequestItem* res = *m_mapResource.find(rid);
        // TODO resource leak if not completed
        res->decrement();
        if (res->refcount()==0) {
            if (m_requests.contains(res)) {
                m_requests.remove(res);
            }
            if (res->getState() == ResourceRequestItem::State::Queued) {
                m_mapResource.erase(m_mapResource.find(res->m_rid));
                delete res;
            }
        }
    }

    //m_cond.wakeOne();
}

void ResourceCache::createRequest(ResourceCache::rid_t rid, QVariant data)
{
    ResourceRequestItem* res;

    // create a new resource request
    res = new ResourceRequestItem(rid,data);
    m_mapResource.insert(rid, res);
    m_requests.insert(res);
    m_cond.wakeOne();


}

// TODO: I havent figured out how to use this to invalidate the cache.
// When requesting a resource, if it exists, have the user validate it.
// if the resource is not valid, request a new resource instead of returning
// it.
// what happens when you are asked to validate an incomplete request?
bool ResourceCache::valid(ResourceCache::rid_t rid, QVariant data, QVariant resource)
{
    Q_UNUSED(rid);
    Q_UNUSED(data);
    Q_UNUSED(resource);
    return true;
}

// not thread safe
ResourceRequestItem* ResourceCache::acquire()
{
    ResourceRequestItem* res = nullptr;
    //qDebug() << m_requests.size() << " requests remain";
    for (ResourceRequestItem* tmp : m_requests) {
        res = tmp;
    }
    if (res!=nullptr)
        m_requests.remove(res);
    return res;
}

void ResourceCache::doNotify(ResourceCache::rid_t id, QVariant resource) {
    //qDebug() << "do notify" << id;
    emit notify(id, resource);
}

} // namespace qtcommon
} // namespace yue
