#include "yue/qtcommon/AlbumArtImage.h"
#include "yue/core/RandomArtExpressions.h"
#include <QSvgRenderer>
#include <QThreadPool>
#include <QtConcurrent>
#include <QDebug>

#include "yue/bell/library.hpp"

namespace yue {
namespace qtcommon {

QVariant ImageCacheThread::loadResource(ResourceCache::rid_t rid, QVariant userData)
{

    // QString seedString = "none";
    // seedString.toUtf8().constData()
    //qDebug() << "loading resource" << rid;
    yue::bell::Database::uid_t artist;
    yue::bell::Database::uid_t album;
    QString path;

    yue::bell::Library::instance()->getArtInfo(rid,artist,album,path);

    yue::core::Random rnd;
    rnd.seed(album);
    yue::core::art::Expr* expr_r = yue::core::art::Expr::create(rnd);
    yue::core::art::Expr* expr_g = yue::core::art::Expr::create(rnd);
    yue::core::art::Expr* expr_b = yue::core::art::Expr::create(rnd);

    /*
    int size = width()/2;
    if (size > 256)
        size = 256;
    if (size < 8)
        size = 8;
    */
    int size = -1;

    if (userData.isValid()) {
        size = userData.toSize().width()/4;
    }

    if (size < 8) {
        size = 256;
    }

    int width = size*2 + 1;

    QImage image(width,width,QImage::Format_ARGB32);


    for (int px=0; px < width; px++) {
        for (int py=0; py < width; py++) {
            float x = static_cast<float>(px - size) / size;
            float y = static_cast<float>(py - size) / size;
            float r = expr_r->eval(x,y);
            float g = expr_g->eval(x,y);
            float b = expr_b->eval(x,y);

            int ir = static_cast<int>(r * 127.5 + 127.5);
            int ig = static_cast<int>(g * 127.5 + 127.5);
            int ib = static_cast<int>(b * 127.5 + 127.5);

            uint c = qRgb(ir,ig,ib);
            image.setPixel(px,py,c);
        }
    }

    delete expr_r;
    delete expr_g;
    delete expr_b;

    QSvgRenderer renderer(QString(":/shared/images/00_file_song.svg"));
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    renderer.render(&painter);

    return image;
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
        update();
    }

    emit seedChanged();
}

void AlbumArtImage::paint(QPainter * painter)
{
    // reload the album art at the new size
    // QQuickPaintedItem does not seem to have any resize signals
    if (width() != m_previousWidth) {
        m_previousWidth = width();
        m_future = QtConcurrent::run(this,&AlbumArtImage::regenerate,true);
    }

    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform,true);

    painter->drawImage(0,0,m_img.scaledToWidth(width()));
}

void AlbumArtImage::doRegenerate()
{
    m_future = QtConcurrent::run(this,&AlbumArtImage::regenerate,true);
}

void AlbumArtImage::regenerate(bool bUpdate)
{

    //if (bUpdate)
    emit doUpdate(QRect());
}

void AlbumArtImage::onNotify(ResourceCache::rid_t id, QVariant data)
{
    if (id == m_seed){
        m_img = data.value<QImage>().copy();
        //if (bUpdate)
        emit doUpdate(QRect());
    }
}


} // namespace qtcommon
} // namespace yue
