#include <QCoreApplication>
#include <QTimer>
#include "test_library.h"
#include "test_playlist.h"

void Task::run()
{
    // Do processing here
    test_library();
    test_playlist();
    emit finished();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Task *task = new Task(&a);

    QObject::connect(task, &Task::finished, &a, &QCoreApplication::quit);

    // This will run the task from the application event loop.
    QTimer::singleShot(0, task, SLOT(run()));

    return a.exec();
}
