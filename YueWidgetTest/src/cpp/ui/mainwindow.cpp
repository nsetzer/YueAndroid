
#include <QVBoxLayout>
#include <QPushButton>

#include "ui/mainwindow.h"
#include "ui/page_library.h"

namespace UI {

class MainWindow
{
private:

    QWidget *m_centralWidget;
    QVBoxLayout *m_centralLayout;
    PageLibrary *m_view;

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

    m_view = new PageLibrary(parent);

    m_centralLayout->addWidget(m_view);

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
