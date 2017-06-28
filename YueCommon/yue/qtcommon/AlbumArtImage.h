#ifndef YUE_QTCOMMON_ALBUMARTIMAGE_H
#define YUE_QTCOMMON_ALBUMARTIMAGE_H

#include <QQuickPaintedItem>
#include <QPainter>
#include <QImage>
#include "yue/global.h"

namespace yue {
namespace qtcommon {


//http://doc.qt.io/qt-5/qquickpainteditem.html
class YUECOMMON_EXPORT AlbumArtImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString seedString READ seedString WRITE setSeedString NOTIFY seedStringChanged)
    QImage m_img;
    QImage m_icon;
public:
    AlbumArtImage();
    virtual ~AlbumArtImage() = default;

    void paint(QPainter *);
    
    QString seedString() { return m_seedString; }

    void setSeedString(QString seedString) {
        m_seedString = seedString;
        emit seedStringChanged();
    }

signals:
    void seedStringChanged();

public slots:
    Q_INVOKABLE void regenerate();

private:
    QString m_seedString;
};

} // namespace qtcommon
} // namespace yue

#endif // ALBUMARTIMAGE_H
