#ifndef YUE_BELL_ALBUMART_H
#define YUE_BELL_ALBUMART_H


#include "yue/bell/library.hpp"
#include "yue/global.h"
#include <QImage>

namespace yue {
namespace bell {

class AlbumArt
{
    static int DEFAULT_WIDTH;
public:
    AlbumArt() = default;
    ~AlbumArt() = default;

    static QImage getAlbumArt(Library* lib, Database::uid_t songid, QSize size = QSize());

private:
    static QImage loadAlbumArtFromPath(QString imagePath, int width);
    static QImage loadAlbumArt(QString imagePath, int width);

    static QImage createDefaultAlbumArt(Database::uid_t albumid, int width);
};


} // namespace bell
} // namespace yue


#endif // ALBUMART_H
