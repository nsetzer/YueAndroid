#ifndef YUE_QTCOMMON_ICONBUTTON_H
#define YUE_QTCOMMON_ICONBUTTON_H

#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QIcon>
#include <QDebug>

namespace yue {
namespace qtcommon {


class IconButton : public QLabel
{
public:
        //static int LargeButton;
        //static int MediumButton;

    enum class IconSize {
        DEFAULT=0,
        LARGE=1,
    };

private:

    Q_OBJECT
    QIcon m_ico;
    int m_w;
    int m_h;

    IconSize m_size;



public:



    IconButton(const QIcon& ico, QWidget *parent = nullptr)
        : QLabel(parent)
        , m_ico(ico)
    {
        m_size = IconSize::DEFAULT;
        init();
    }

    IconButton(IconSize size, const QIcon& ico, QWidget *parent = nullptr)
        : QLabel(parent)
        , m_ico(ico)
    {
        m_size = size;
        init();
    }


    void setIcon(const QIcon& ico) {
        m_ico = ico;
        init();
    }

    virtual QSize sizeHint() const override {
        return QSize(m_w, m_h);
    }

    virtual void mousePressEvent(QMouseEvent *event) override {
        Q_UNUSED(event);

        setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

        qDebug() << size() << sizeHint() << QFontMetrics(font()).height();

        auto desktop = QApplication::desktop();

        qDebug() << "screen" << desktop->logicalDpiX() << desktop->logicalDpiY() << desktop->availableGeometry(desktop->primaryScreen());

    }

    virtual void mouseReleaseEvent(QMouseEvent *event) override {
        Q_UNUSED(event);
        setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
        emit clicked();
    }

signals:
    void clicked();

private:
    void init() {
        // TODO: make size an enum

        auto desktop = QApplication::desktop();

#ifdef Q_OS_ANDROID
        // this was chosen to make the buttons easier to hit
        // on android
        // for a large icon use  3 / 4
        float factor = (m_size==IconSize::DEFAULT)?0.5F:0.75F;
#else
        // this was chosen be reading off the screen...
        // the defaul toolbutton size is about 32 px
        // the default resolution was 96 dpi.
        // for a large icon use / 2
        //float factor = (size==0)?(1.0F/3.0F):0.5F;
        float factor = 1.0F/3.0F;
#endif
        m_w = static_cast<int>(desktop->logicalDpiX() * factor);
        m_h = static_cast<int>(desktop->logicalDpiY() * factor);

        setMinimumSize(m_w, m_h);
        setFixedSize(m_w, m_h);

        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
        setLineWidth(1);

        setPixmap(m_ico.pixmap(sizeHint()));
        setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
};


} // qtcommon
} // yue


#endif

