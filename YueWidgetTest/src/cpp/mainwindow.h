#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace UI {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QScopedPointer<UI::MainWindow> m_ui;
};

#endif // MAINWINDOW_H
