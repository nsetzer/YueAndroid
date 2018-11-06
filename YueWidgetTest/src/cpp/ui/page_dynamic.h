#ifndef UI_PAGE_DYNAMIC_H
#define UI_PAGE_DYNAMIC_H

#include <QScopedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QProgressBar>
#include <QLabel>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QVariant>
#include <QGroupBox>
#include <QScrollArea>
#include <QLineEdit>

namespace UI {

class uiPageDynamic;

} // namespace UI

class PageDynamic : public QWidget
{
    Q_OBJECT

private:

public:

    PageDynamic(QWidget *parent = nullptr);
    ~PageDynamic();

private:
    QScopedPointer<UI::uiPageDynamic> m_ui;
};

#endif
