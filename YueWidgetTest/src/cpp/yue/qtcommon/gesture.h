#ifndef YUE_QTCOMMON_GESTURE_H
#define YUE_QTCOMMON_GESTURE_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QScrollBar>
#include <QMouseEvent>
#include <QPainter>


namespace yue {
namespace qtcommon {


class Gesture : public QObject
{
    Q_OBJECT
public:
    explicit Gesture(QObject *parent = nullptr);
    virtual ~Gesture() {}

    enum class Direction {
        Unknown,
        Up,
        Right,
        Down,
        Left
    };

    void setViewport(QWidget *viewport) { m_pViewport = viewport; }
    void setHScrollBar(QScrollBar *bar) { m_pHBar = bar; }
    void setVScrollBar(QScrollBar *bar) { m_pVBar = bar; }

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void swipe(Direction dir);
    void swipe_update(float dx, float dy);
    void drag(int dx, int dy);
    void tap(int x, int y);
    void long_press(int x, int y);
    void long_move(int x, int y);
    void long_release(int x, int y);
    void auto_scroll(int dx, int dy);


private slots:

    void onSwipeTimeout();
    void onPressTimeout();
    void onAutoscrollTimeout();

protected:

    QWidget *m_pViewport = nullptr;
    QScrollBar *m_pHBar = nullptr;
    int m_pHBar_pos = 0;
    QScrollBar *m_pVBar = nullptr;
    int m_pVBar_pos = 0;

    QTimer m_timerSwipe;
    QTimer m_timerPress;
    QTimer m_timerAutoscroll;

    int m_t_swipe = 300; // ms
    int m_t_long_press = 750; // ms
    int m_t_frame_rate = 60; // frames per second
    int m_t_update_interval = 16; // ms, 1000 / frame_rate

private:

    virtual float decay(int i);

    // time for last press, move, release
    QTime m_tPress;
    int m_t_press = 0; //deprecated
    int m_t_move = 0;
    int m_t_release = 0;

    // position of mouse press, move, release
    int m_p_x = 0;
    int m_p_y = 0;
    int m_m_x = 0;
    int m_m_y = 0;
    int m_r_x = 0;
    int m_r_y = 0;

    // metrics
    int m_distance_total = 0;
    int m_distance_x = 0;
    int m_distance_y = 0;
    int m_delta_x = 0;
    int m_delta_y = 0;

    float m_swipe_power_x = 0.F;
    float m_swipe_power_y = 0.F;
    int m_swipe_update_index = 0;

    bool m_is_tap = false;
    bool m_is_long = false;

    // indicate autoscroll direction using -1, 0, 1
    int m_autoscroll_x = 0;
    int m_autoscroll_y = 0;
    float m_autoscroll_margin = 0.15F;

    void move_event_default(QMouseEvent *event);
    void move_event_long(QMouseEvent *event);

    void release_event_default(QMouseEvent *event);
    void release_event_long(QMouseEvent *event);

    void _move_slider(QScrollBar *pBar, int delta);

};


} // qtcommon
} // yue

#endif // GESTURE_H
