#ifndef YUE_QTCOMMON_SVG_H
#define YUE_QTCOMMON_SVG_H

#include <QObject>
#include <QSvgRenderer>
#include <QPainter>
#include <QDebug>

// TODO: create a cache for resources,
// use an enum class for icons, e.g. SvgRender::Icon::MORE -> ":/res/more.svg"

namespace yue {
namespace qtcommon {


class SvgRender : public QObject
{

 public:
    enum class Icon {
        UNKOWN,
        MORE
    };

    SvgRender()
        : QObject()
    {}


    QString iconPath(SvgRender::Icon icon) {
        Q_UNUSED(icon);
        return "";
    }

    /*
     * Paint an svg to fill a given rectangle, centered horizontally and vertically
     * the svg is scaled to fit
     */
    void paint(QString icon_path, QPainter *painter, const QRect& viewbox)
    {
        QSvgRenderer renderer(icon_path);

        QRectF boxF = renderer.viewBox();

        double factor = 1.0; // qMin(viewbox.width() / boxF.width(), viewbox.height() / boxF.height());

        QRectF box(0.0, 0.0, boxF.width() * factor, boxF.height() * factor);
        renderer.setViewBox(box);

        int l = viewbox.left() + viewbox.width()/2 - box.width()/2;
        int t = viewbox.top() + viewbox.height()/2 - box.height()/2;

        renderer.render(painter, QRect(l, t, box.width(), box.height()));


    }
};

} // qtcommon
} // yue

#endif // YUE_QTCOMMON_SVG_H
