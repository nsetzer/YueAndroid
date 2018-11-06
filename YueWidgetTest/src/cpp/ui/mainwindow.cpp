
#include <QVBoxLayout>
#include <QPushButton>

#include "ui/mainwindow.h"
#include "ui/page_library.h"
#include "ui/page_queue.h"
#include "ui/page_player.h"
#include "ui/page_explorer.h"

namespace UI {

class MainWindow
{
private:

    QWidget *m_centralWidget;
    QVBoxLayout *m_centralLayout;

    QTabWidget *m_tabPages;
    PageLibrary *m_pageLibrary;
    PagePlayer *m_pagePlayer;
    PageQueue *m_pageQueue;
    PageExplorer *m_pageExplorer;

public:
    explicit MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();
};

MainWindow::MainWindow(QMainWindow *parent)
{
    m_centralWidget = new QWidget(parent);
    parent->setCentralWidget(m_centralWidget);

    m_centralLayout = new QVBoxLayout();
    m_centralWidget->setLayout(m_centralLayout);

    m_tabPages = new QTabWidget(parent);

    m_pageQueue = new PageQueue(parent);
    m_pagePlayer = new PagePlayer(parent);
    m_pageLibrary = new PageLibrary(parent);
    m_pageExplorer = new PageExplorer(parent);

    m_tabPages->addTab(m_pageQueue, "queue");
    m_tabPages->addTab(m_pageLibrary, "library");

    m_tabPages->addTab(new QWidget(parent), "playlist");
    m_tabPages->addTab(m_pageExplorer, "files");
    m_tabPages->addTab(new QWidget(parent), "settings");

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
}

MainWindow::~MainWindow()
{
}
