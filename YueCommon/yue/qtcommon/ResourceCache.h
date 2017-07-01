
#ifndef YUE_QTCOMMON_RESOURCECACHE_H
#define YUE_QTCOMMON_RESOURCECACHE_H

#include <QMetaType>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>
#include <QSet>
#include <QList>
#include <QMap>
#include <QVariant>

#include <exception>


#include "yue/core/lrucache.h"

namespace yue {
namespace qtcommon {

class ResourceCacheThread;
class ResourceCache;
class ResourceRequestItem;

class ResourceCache : public QObject
{
    Q_OBJECT;

public:
    typedef size_t rid_t;
    ResourceCache(int numThreads, size_t cacheSize, QObject* parent = nullptr);
    ~ResourceCache();

    QVariant request(ResourceCache::rid_t rid, QVariant data=QVariant());
    void revoke(ResourceCache::rid_t);

    virtual bool valid(ResourceCache::rid_t rid, QVariant data, QVariant resource);

protected:
    virtual ResourceCacheThread* newWorkerThread() = 0;

private:
    bool m_alive;
    QMutex m_mutex;
    QWaitCondition m_cond;

    int m_numThreads;
    size_t m_cacheSize;
    // maintain a list of requests to process
    QSet<ResourceRequestItem*> m_requests;
    // map request identifiers to their requests.
    QMap<ResourceCache::rid_t,ResourceRequestItem*> m_mapResource;
    // maintain a cache of completed requests
    yue::core::lrucache<ResourceRequestItem*> m_cache;
    std::vector<ResourceCacheThread*> m_workerThreads;


    void init();
    ResourceRequestItem* acquire();
    void doNotify(ResourceCache::rid_t id, QVariant resource);

signals:
    void notify(ResourceCache::rid_t id, QVariant resource);

    friend class ResourceCacheThread;

};


class ResourceCacheThread : public QThread
{
    Q_OBJECT
public:
    ResourceCache* m_pCache;
    ResourceCacheThread(ResourceCache* parent)
        : QThread()
        , m_pCache(parent)
    {}
    ~ResourceCacheThread() = default;

    void join();

protected:
    // override in a subclass
    virtual QVariant loadResource(ResourceCache::rid_t rid, QVariant userData) = 0;

private:
    void run() override;

    bool run_main(ResourceCache::rid_t rid, QVariant userData, QVariant* resource);


};

/**
 * ResourceRequestItem holds the state of a requested resource
 *
 * This is an internal data structure used by the Resource Cache
 * it holds the state of the request (including number of subscribers)
 * and the loaded resource once it is ready.
 */
class ResourceRequestItem
{
public:

    enum class State {
        Queued=1,
        Complete=2,
        Error=3,
    };

    ResourceRequestItem(ResourceCache::rid_t id, QVariant userData)
        : m_state(State::Queued)
        , m_rid(id)
        , m_resource()
        , m_userData(userData)
        , m_refcount(1)
    {}
    virtual ~ResourceRequestItem() = default;

    void increment() { m_refcount++; }
    void decrement() { m_refcount--; }
    int refcount() { return m_refcount; }

    void setState(State state) { m_state = state; }
    State getState() { return m_state; }

    State m_state;
    ResourceCache::rid_t m_rid;
    QVariant m_resource;
    QVariant m_userData;
    int m_refcount;

};



} // namespace qtcommon
} // namespace yue

//note: this is a duplicate of Database::uid_t as far as Qt is concerned.
//Q_DECLARE_METATYPE(yue::qtcommon::ResourceCache::rid_t);

#endif // YUE_QTCOMMON_RESOURCECACHE_H
