#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScopedPointer>
#include <QVBoxLayout>
#include <QPushButton>

namespace UI {

class MainWindow
{
    QWidget *m_centralWidget;
    QVBoxLayout *m_centralLayout;
    QPushButton *m_button;
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

    m_button = new QPushButton("Click Me");

    m_centralLayout->addWidget(m_button);

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
