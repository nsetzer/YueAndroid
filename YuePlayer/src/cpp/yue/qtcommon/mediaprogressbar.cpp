#include "yue/qtcommon/mediaprogressbar.h"


namespace yue {
namespace qtcommon {


MediaProgressBar::MediaProgressBar(QWidget *parent)
    : QWidget(parent)
    , m_fontMetrics(QFontMetrics(QApplication::font("QLabel")))
    , m_text("")
    , m_position(0.F)
    , m_trackingPosition(0.F)
    , m_isTracking(false)
    , m_length(0)

{
    setFixedHeight(static_cast<int>(m_fontMetrics.height() * 1.5F));
}

void MediaProgressBar::setText(QString text)
{
    m_text = text;
    update();
}
QString MediaProgressBar::getText() const
{
    return m_text;
}

void MediaProgressBar::setLength(int ms)
{
    m_length = ms;
    update();
}
int MediaProgressBar::getLength() const
{
    return m_length;
}

void MediaProgressBar::setPosition(float value)
{
    m_position = value;
    update();
}
float MediaProgressBar::getPosition() const
{
    return m_position;
}

void MediaProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int fh = m_fontMetrics.height();

    //QRect rect(0, 0, width() - 1, height() - 1);
    //painter.drawRect(rect);

    QRect fontRect(0, 0, width(), fh);
    painter.drawText(fontRect, Qt::AlignVCenter|Qt::AlignHCenter, m_text);

    int h = m_fontMetrics.height() / 3;
    int y = height() - h - 1;
    float p = (m_isTracking)?m_trackingPosition:m_position;
    int w = static_cast<int>(p * width()-2);
    painter.drawRect(0, y, width()-1, h);
    painter.fillRect(1, y+1, w, h-1, QBrush(QColor(70, 130, 180)));
}


void MediaProgressBar::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_isTracking = true;
}

void MediaProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    m_trackingPosition = event->x() / static_cast<float>(width());
    update();
}

void MediaProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    float pos = event->x() / static_cast<float>(width());
    emit positionChanged(pos);
    m_isTracking = false;
}


} // qtcommon
} // yue

