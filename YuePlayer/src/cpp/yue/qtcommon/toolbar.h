#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

namespace yue {
namespace qtcommon {

class ToolBar : public QToolBar
{
    int m_defaultSize;
    float m_factor;
public:
    ToolBar(QWidget *parent=nullptr);
    ToolBar(float factor, QWidget *parent=nullptr);

    // set the icon scale factor (1.0 is the default font size)
    // ignored on all platforms other than android
    void setScaleFactor(float factor);

    float scaleFactor() const { return m_factor; }
 };

} // qtcommon
} // yue


#endif // TOOLBAR_H
