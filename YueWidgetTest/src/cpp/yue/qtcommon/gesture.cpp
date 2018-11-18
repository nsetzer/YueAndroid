#include "gesture.h"


namespace yue {
namespace qtcommon {


Gesture::Gesture(QObject *parent)
    : QObject(parent)
    , m_timerSwipe(this)
    , m_timerPress(this)
    , m_timerAutoscroll(this)
{

    m_timerSwipe.setInterval(m_t_update_interval);
    m_timerSwipe.setSingleShot(false);
    connect(&m_timerSwipe, &QTimer::timeout,
            this, &Gesture::onSwipeTimeout);

    m_timerPress.setInterval(m_t_long_press);
    m_timerPress.setSingleShot(true);
    connect(&m_timerPress, &QTimer::timeout,
            this, &Gesture::onPressTimeout);

    m_timerAutoscroll.setInterval(m_t_update_interval);
    m_timerAutoscroll.setSingleShot(false);
    connect(&m_timerAutoscroll, &QTimer::timeout,
            this, &Gesture::onAutoscrollTimeout);
}

void Gesture::mousePressEvent(QMouseEvent *event)
{
    m_tPress.start();
    m_timerPress.start();

    m_m_x = m_p_x = event->x();
    m_m_y = m_p_y = event->y();

    m_is_tap = true;
    m_is_long = false;

    if (m_pHBar) {
        m_pHBar_pos = m_pHBar->sliderPosition();
    }

    if (m_pVBar) {
        m_pVBar_pos = m_pVBar->sliderPosition();
    }
}

void Gesture::mouseMoveEvent(QMouseEvent *event)
{
    if (m_is_long) {
        move_event_long(event);
    } else {
        move_event_default(event);
    }
}

void Gesture::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_is_long) {
        release_event_long(event);
    } else {
        release_event_default(event);
    }
}

void Gesture::move_event_default(QMouseEvent *event)
{
    m_timerPress.stop();

    m_t_move = m_tPress.elapsed();

    // measure the distance traveled by this event
    m_distance_x += qAbs(event->x() - m_m_x);
    m_distance_y += qAbs(event->y() - m_m_y);
    m_distance_total = m_distance_x + m_distance_y;

    m_m_x = event->x();
    m_m_y = event->y();

    int dx = event->x() - m_p_x;
    int dy = event->y() - m_p_y;
    // indicate a drag has occurred, canceling a tap
    emit drag(dx, - dy);

    if (m_pHBar) {
        int pos = qMax(m_pHBar->minimum(), qMin(m_pHBar->maximum(), m_pHBar_pos + dx));
        m_pHBar->setSliderPosition(pos);
    }

    if (m_pVBar) {
        int pos = qMax(m_pVBar->minimum(), qMin(m_pVBar->maximum(), m_pVBar_pos - dy));
        m_pVBar->setSliderPosition(pos);
    }

    m_is_tap = false;
}

void Gesture::move_event_long(QMouseEvent *event)
{

}

void Gesture::release_event_default(QMouseEvent *event)
{
    m_timerPress.stop();

    m_t_release = m_tPress.elapsed();  // ms elapsed

    m_r_x = event->x();
    m_r_y = event->y();

    m_delta_x = m_r_x - m_p_x;
    m_delta_y = m_r_y - m_p_y;


    m_distance_x += qAbs(m_r_x - m_m_x);
    m_distance_y += qAbs(m_r_y - m_m_y);
    m_distance_total = m_distance_x + m_distance_y;

    int elapsed = m_t_release;
    if (m_is_tap && elapsed < m_t_swipe) {
        emit tap(m_r_x, m_r_y);
        // cancel the swipe
        m_swipe_power_x = 0;
        m_swipe_power_y = 0;
    } else if (elapsed < m_t_swipe) {
        // x/y pixels per second
        float xpps = 1000.F * static_cast<float>(m_delta_x)/elapsed;
        float ypps = 1000.F * static_cast<float>(m_delta_y)/elapsed;

        Direction dir = Direction::Unknown;
        if (qAbs(xpps) > qAbs(ypps)) {
            dir = (xpps>0)?Direction::Right:Direction::Left;
        } else {
            dir = (ypps>0)?Direction::Down:Direction::Up;
        }

        emit swipe(dir);

        m_swipe_power_x = xpps;
        m_swipe_power_y = ypps;
        m_swipe_update_index = 0;
        m_timerSwipe.start();
    }
}

void Gesture::release_event_long(QMouseEvent *event)
{

}

float Gesture::decay(int i) {
    if ( i < m_t_frame_rate )
        return 1.0F;
    return 0.90F;
}

void Gesture::onSwipeTimeout()
{
    float f = decay(m_swipe_update_index++);
    m_swipe_power_x *= f;
    m_swipe_power_y *= f;

    emit swipe_update(m_swipe_power_x / m_t_frame_rate,
                      - m_swipe_power_y / m_t_frame_rate);

    if (m_pHBar != nullptr) {
        int dx = static_cast<int>(m_swipe_power_x / m_t_frame_rate);
        _move_slider(m_pHBar, dx);
    }

    if (m_pVBar != nullptr) {
        int dy = static_cast<int>(m_swipe_power_y / m_t_frame_rate);
        _move_slider(m_pVBar, - dy);
    }

    if (qAbs(m_swipe_power_x) < m_t_frame_rate) {
        m_swipe_power_x = 0.F;
    }

    if (qAbs(m_swipe_power_y) < m_t_frame_rate) {
        m_swipe_power_y = 0.F;
    }

    if (m_swipe_power_x == 0.F && m_swipe_power_y == 0.F) {
        m_timerSwipe.stop();
    }
}

void Gesture::onPressTimeout()
{

}

void Gesture::onAutoscrollTimeout()
{

}

void Gesture::_move_slider(QScrollBar *pBar, int delta)
{
    int pos = pBar->sliderPosition() + delta;
    pos = qMax(pBar->minimum(), qMin(pBar->maximum(), pos));
    pBar->setSliderPosition(pos);
}


} // qtcommon
} // yue
