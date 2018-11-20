#include "toolbar.h"

#include <QApplication>
#include <QLayout>

namespace yue {
namespace qtcommon {


ToolBar::ToolBar(QWidget *parent)
    : QToolBar (parent)
{

    // 12 is an arbitrary minimum font size
    m_defaultSize = qMax(12, QFontMetrics(QApplication::font()).height());

    setScaleFactor(1.5);
}

ToolBar::ToolBar(float factor, QWidget *parent)
    : QToolBar (parent)
{

    // 12 is an arbitrary minimum font size
    m_defaultSize = qMax(12, QFontMetrics(QApplication::font()).height());

    setScaleFactor(factor);
}


void ToolBar::setScaleFactor(float factor)
{
#ifdef Q_OS_ANDROID

    // round the size to a multiple of 8 to help prevent strange
    // scaling artifacts in icons
    int size = static_cast<int>((m_defaultSize * m_factor)/8.0F)*8;

    //setIconSize(QSize(size, size));
    // increase the spacing between buttons to make it easier to click
    //layout()->setSpacing(m_defaultSize / 2);

#endif

    m_factor = factor;
}


} // qtcommon
} // yue

