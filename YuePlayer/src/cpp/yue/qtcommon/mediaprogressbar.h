#ifndef YUE_QTCOMMON_MEDIAPROGRESSBAR_H
#define YUE_QTCOMMON_MEDIAPROGRESSBAR_H

#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QIcon>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

#include <QWidget>

namespace yue {
namespace qtcommon {

class MediaProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit MediaProgressBar(QWidget *parent = nullptr);

    void setText(QString text);
    QString getText() const;

    void setLength(int ms);
    int getLength() const;

    void setPosition(float value);
    float getPosition() const;

signals:
    void positionChanged(float pos);

public slots:

protected:
    virtual void paintEvent(QPaintEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QFontMetrics m_fontMetrics;
    QString m_text;
    float m_position;
    float m_trackingPosition;
    bool m_isTracking;
    int m_length;


};


} // qtcommon
} // yue


#endif // YUE_QTCOMMON_MEDIAPROGRESSBAR_H
