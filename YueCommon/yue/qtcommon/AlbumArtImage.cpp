#include "yue/qtcommon/AlbumArtImage.h"
#include "yue/core/RandomArtExpressions.h"
#include <QSvgRenderer>

namespace yue {
namespace qtcommon {


AlbumArtImage::AlbumArtImage()
    : QQuickPaintedItem()
{
    setFillColor(QColor(128,128,0));

    //regenerate();

    connect(this,&AlbumArtImage::seedStringChanged,
            this,&AlbumArtImage::regenerate);

}

void AlbumArtImage::paint(QPainter * painter)
{
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform,true);
    if (!m_img.isNull())
        painter->drawImage(0,0,m_img.scaledToWidth(width()));
    //painter->drawImage(0,0,m_icon.scaledToWidth(width()));
}

void AlbumArtImage::regenerate()
{
    yue::core::Random rnd;
    rnd.seed(m_seedString.toUtf8().constData());
    yue::core::art::Expr* expr_r = yue::core::art::Expr::create(rnd);
    yue::core::art::Expr* expr_g = yue::core::art::Expr::create(rnd);
    yue::core::art::Expr* expr_b = yue::core::art::Expr::create(rnd);

    int size = width()/2;
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

    QSvgRenderer renderer(QString(":/shared/images/00_file_song.svg"));
    //int size = 128;
    //int width = size*2 + 1;
    //QImage image(width, width, QImage::Format_ARGB32);
    //image.fill(0x00000000);
    QPainter painter(&image);
    renderer.render(&painter);
    //m_icon = image;

    m_img = image;

    delete expr_r;
    delete expr_g;
    delete expr_b;

    update();

}

} // namespace qtcommon
} // namespace yue
