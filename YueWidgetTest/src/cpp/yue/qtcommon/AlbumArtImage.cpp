#include "yue/qtcommon/AlbumArtImage.h"
#include "yue/bell/AlbumArt.h"

#include "yue/bell/library.hpp"

namespace yue {
namespace qtcommon {

QVariant ImageCacheThread::loadResource(ResourceCache::rid_t rid, QVariant userData)
{
    return yue::bell::AlbumArt::getAlbumArt(yue::bell::Library::instance(), rid, userData.toSize());
}

/**
 * @brief validate that the given resource was constructed using the given rid and userData.
 * @param rid
 * @param data
 * @param resource
 * @return false to request loading the resource again. or true if the resource
 *         is acceptable.
 */
bool ImageCache::valid(ResourceCache::rid_t rid, QVariant userData, QVariant resource)
{
    Q_UNUSED(rid);
    int width = userData.toSize().width();
    QImage img = resource.value<QImage>();
    return img.width() == width;
}

ImageCache AlbumArtImage::m_cache(4,100);

AlbumArtImage::AlbumArtImage()
    : QQuickPaintedItem()
    , m_seed(0)
{

    connect(&m_cache,&ResourceCache::notify,
                this,&AlbumArtImage::onNotify);

    m_img = QImage(128,128,QImage::Format_ARGB32);
    m_img.fill(QColor(200,0,0));



    //connect(this,&AlbumArtImage::seedChanged,
    //        this,&AlbumArtImage::doRegenerate);
    connect(this,&AlbumArtImage::doUpdate,
            this,&AlbumArtImage::update);
}

AlbumArtImage::~AlbumArtImage()
{
    m_cache.revoke(m_seed);
}

void AlbumArtImage::setSeed(int seed) {

    if (m_seed != 0) {
        m_cache.revoke(m_seed);
    }
    m_seed = seed;

    QSize size = QSize(width(),height());

    QVariant data = m_cache.request(m_seed,size);

    if (data.isValid()) {
        m_img = data.value<QImage>();
        m_imgScaled = QImage();
        update();
    } else {
        m_img = QImage();
        m_imgScaled = m_img;
    }

    emit seedChanged();
}

void AlbumArtImage::paint(QPainter * painter)
{
    // reload the album art at the new size
    // QQuickPaintedItem does not seem to have any resize signals
    if ((m_imgScaled.isNull() || width() != m_previousWidth) &&
        !m_img.isNull()) {
        m_previousWidth = width();
        m_imgScaled = m_img.scaledToWidth(width());
    }

    if (!m_imgScaled.isNull() ) {
        painter->setRenderHint(QPainter::Antialiasing,true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform,true);
        painter->drawImage(0,0,m_imgScaled);
    } else {
        painter->fillRect(0,0,width(),height(),QColor(0,0,0));
    }
}

void AlbumArtImage::onNotify(ResourceCache::rid_t id, QVariant data)
{
    if (id == m_seed){
        m_img = data.value<QImage>();
        m_imgScaled = QImage();
        //if (bUpdate)
        update();
    }
}


} // namespace qtcommon
} // namespace yue
