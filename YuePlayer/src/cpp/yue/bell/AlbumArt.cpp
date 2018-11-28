#include "yue/bell/AlbumArt.h"

#include "yue/core/RandomArtExpressions.h"

#include <QPainter>
#include <QSvgRenderer>
#include <QThreadPool>
#include <QtConcurrent>
#include <QDebug>

#include <functional>

namespace yue {
namespace bell {

int AlbumArt::DEFAULT_WIDTH = 256;

//static
QImage AlbumArt::getAlbumArt(Library* lib, Database::uid_t songid, QSize size/* = QSize()*/)
{
    QString artist;
    QString album;
    QString songPath;

    int width = (size.isValid() && size.width()>=16) ? size.width() : AlbumArt::DEFAULT_WIDTH;

    lib->getArtInfo(songid, artist, album, songPath);

    QImage image = AlbumArt::loadAlbumArtFromPath(songPath, width);

    if (image.isNull())
        image = AlbumArt::createDefaultAlbumArt(album, width);

    return image;
}


//static
QImage AlbumArt::loadAlbumArtFromPath(QString imagePath, int width)
{
    QFileInfo fi(imagePath);
    QDir directory = fi.dir();

    QStringList pathAlternatives;
    pathAlternatives.push_back( directory.filePath(fi.completeBaseName() + ".jpg") );
    pathAlternatives.push_back( directory.filePath(fi.completeBaseName() + ".png") );
    pathAlternatives.push_back( directory.filePath("folder.jpg") );
    pathAlternatives.push_back( directory.filePath("folder.png") );
    pathAlternatives.push_back( directory.filePath("cover.jpg") );
    pathAlternatives.push_back( directory.filePath("cover.png") );

    for (QString path : pathAlternatives ) {
        QFileInfo fi_img(path);
        if (fi_img.exists()) {
            return AlbumArt::loadAlbumArt(path,width);
        }
    }

    return QImage();
}

//static
QImage AlbumArt::loadAlbumArt(QString imagePath, int width)
{
    QImage image = QImage(imagePath);
    if (!image.isNull())
        return image.scaled(width, width, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return image;
}


//static
QImage AlbumArt::createDefaultAlbumArt(QString album, int width)
{

    int size = width/2;

    yue::core::Random rnd;
    // TODO: use an integer hash function...
    rnd.seed(std::hash<std::string>()(album.toStdString()));
    yue::core::art::Expr* expr_r = yue::core::art::Expr::create(rnd);
    yue::core::art::Expr* expr_g = yue::core::art::Expr::create(rnd);
    yue::core::art::Expr* expr_b = yue::core::art::Expr::create(rnd);


    //unsigned char* buffer = new unsigned char[4*width*width];
    QImage image(width,width,QImage::Format_RGB32);

    for (int px=0; px < width; px++) {
        for (int py=0; py < width; py++) {
            float x = static_cast<float>(px - size) / size;
            float y = static_cast<float>(py - size) / size;
            float r = expr_r->eval(x,y);
            float g = expr_g->eval(x,y);
            float b = expr_b->eval(x,y);

            int ir = static_cast<int>(r * 127.5F + 127.5F);
            int ig = static_cast<int>(g * 127.5F + 127.5F);
            int ib = static_cast<int>(b * 127.5F + 127.5F);

            //buffer[4 * (px * width + py)    ] = ir;
            //buffer[4 * (px * width + py) + 1] = ig;
            //buffer[4 * (px * width + py) + 2] = ib;
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


} // namespace bell
} // namespace yue
