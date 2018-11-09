#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QTabWidget>
#include <QKeyEvent>
#include <QDebug>

namespace UI {

class MainWindow;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onSearch(QString query);

private:
    QScopedPointer<UI::MainWindow> m_ui;


protected:

    virtual void keyReleaseEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Back) {
            qDebug() << "key release: back";
        } else if (event->key() == Qt::Key_Forward) {
            qDebug() << "key release: forward";
        } else if (event->key() == Qt::Key_Stop) {
            qDebug() << "key release: stop";
        } else if (event->key() == Qt::Key_MediaStop) {
            qDebug() << "key release: media stop";
        } else if (event->key() == Qt::Key_MediaPlay) {
            qDebug() << "key release: media play";
        } else if (event->key() == Qt::Key_MediaPrevious) {
            qDebug() << "key release: media previous";
        } else if (event->key() == Qt::Key_MediaNext) {
            qDebug() << "key release: media next";
        } else if (event->key() == Qt::Key_MediaPause) {
            qDebug() << "key release: media pause";
        } else if (event->key() == Qt::Key_MediaTogglePlayPause) {
            qDebug() << "key release: media toggle";
        }
    }


};

#endif // MAINWINDOW_H
