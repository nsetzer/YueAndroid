#ifndef YUE_QTCOMMON_ALBUMARTIMAGE_H
#define YUE_QTCOMMON_ALBUMARTIMAGE_H

#include <QPainter>
#include <QImage>
#include "yue/global.h"
#include <QFuture>
#include "yue/qtcommon/ResourceCache.h"

namespace yue {
namespace qtcommon {

class ImageCacheThread : public ResourceCacheThread
{
public:
    ImageCacheThread(ResourceCache* parent)
        : ResourceCacheThread(parent)
    {}
    ~ImageCacheThread() = default;
    virtual QVariant loadResource(ResourceCache::rid_t rid, QVariant userData) override;
};

class ImageCache : public ResourceCache
{
public:
    ImageCache(int numThreads, size_t cacheSize, QObject* parent = nullptr)
        : ResourceCache(numThreads, cacheSize, parent)
    {};
    ~ImageCache() = default;

    bool valid(ResourceCache::rid_t rid, QVariant userData, QVariant resource);
protected:
    virtual ResourceCacheThread* newWorkerThread() override {
        return new ImageCacheThread(this);
    }
};

//http://doc.qt.io/qt-5/qquickpainteditem.html
class YUECOMMON_EXPORT AlbumArtImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int seed READ seed WRITE setSeed NOTIFY seedChanged)
    QImage m_img;
    QImage m_imgScaled;

    static ImageCache m_cache;
public:
    AlbumArtImage();
    virtual ~AlbumArtImage();

    void paint(QPainter *);
    
    int seed() { return m_seed; }

    void setSeed(int seedString);

signals:
    void seedChanged();
    void doUpdate(const QRect &rect);

public slots:
    void onNotify(ResourceCache::rid_t id, QVariant data);
private:

    int m_seed;
    qreal m_previousWidth=-1;
    QFuture<void> m_future;
};

} // namespace qtcommon
} // namespace yue

#endif // ALBUMARTIMAGE_H
