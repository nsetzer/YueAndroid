
#include <QVBoxLayout>
#include <QPushButton>

#include "ui/mainwindow.h"
#include "ui/page_library.h"
#include "ui/page_queue.h"
#include "ui/page_player.h"
#include "ui/page_explorer.h"
#include "ui/page_settings.h"
#include "ui/page_dynamic.h"
#include "ui/page_remote.h"

/**
create a custom icon button
 - on the resize event, resize to (min, min)
    - keep the aspect ratio square
*/

namespace UI {

class MainWindow
{
public:

    QWidget *m_centralWidget;
    QVBoxLayout *m_centralLayout;

    QTabWidget *m_tabPages;
    PageLibrary *m_pageLibrary;
    PagePlayer *m_pagePlayer;
    PageQueue *m_pageQueue;
    PageExplorer *m_pageExplorer;
    PageSettings *m_pageSettings;
    PageDynamic *m_pageDynamic;
    PageRemote *m_pageRemote;

    explicit MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();
};

MainWindow::MainWindow(QMainWindow *parent)
{
    m_centralWidget = new QWidget(parent);
    parent->setCentralWidget(m_centralWidget);

    m_centralLayout = new QVBoxLayout();
    m_centralLayout->setContentsMargins(0, 0, 0, 0);
    m_centralWidget->setLayout(m_centralLayout);

    m_tabPages = new QTabWidget(parent);

    m_pageQueue = new PageQueue(parent);
    m_pagePlayer = new PagePlayer(parent);
    m_pageLibrary = new PageLibrary(parent);
    m_pageExplorer = new PageExplorer(parent);
    m_pageSettings = new PageSettings(parent);
    m_pageDynamic = new PageDynamic(parent);
    m_pageRemote = new PageRemote(parent);

    m_tabPages->addTab(m_pageQueue, "queue");
    m_tabPages->addTab(m_pageLibrary, "library");
    m_tabPages->addTab(m_pageDynamic, "dynamic");
    m_tabPages->addTab(m_pageExplorer, "files");
    m_tabPages->addTab(m_pageSettings, "settings");
    m_tabPages->addTab(m_pageRemote, "remote");

    m_centralLayout->addWidget(m_pagePlayer);
    m_centralLayout->addWidget(m_tabPages);

    return;
}

MainWindow::~MainWindow() {
}

} // namespace UI

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new UI::MainWindow(this))

{
    connect(m_ui->m_pageDynamic, &PageDynamic::search,
            this, &MainWindow::onSearch);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onSearch(QString query)
{
    m_ui->m_tabPages->setCurrentWidget(m_ui->m_pageLibrary);
    m_ui->m_pageLibrary->setQuery(query);
}
