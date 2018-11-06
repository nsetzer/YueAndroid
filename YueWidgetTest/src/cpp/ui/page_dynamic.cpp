
#include "ui/page_dynamic.h"
namespace UI {

class uiPageDynamic
{

public:

    QVBoxLayout *m_layoutCentral;
    QScrollArea *m_scrollArea;

    QWidget *m_centralWidget;
    QVBoxLayout *m_layoutMain;

    QToolButton *m_btnNewItem;

    uiPageDynamic(QWidget *parent = nullptr);
    ~uiPageDynamic();

};

uiPageDynamic::uiPageDynamic(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_scrollArea = new QScrollArea(parent);
    m_centralWidget = new QWidget(parent);
    m_layoutMain = new QVBoxLayout();
    m_btnNewItem = new QToolButton(parent);

    // -
    parent->setLayout(m_layoutCentral);
    m_layoutCentral->addWidget(m_scrollArea);
    m_layoutCentral->addWidget(m_btnNewItem);
    m_centralWidget->setLayout(m_layoutMain);
    //m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->setWidget(m_centralWidget);
    m_scrollArea->setWidgetResizable(true);

}

uiPageDynamic::~uiPageDynamic()
{

}


} // namespace UI

PageDynamic::PageDynamic(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageDynamic(this))
{

}

PageDynamic::~PageDynamic() {
}
