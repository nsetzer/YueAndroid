#ifndef UI_PAGE_SETTINGS_H
#define UI_PAGE_SETTINGS_H

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
#include <QPushButton>

#include "yue/bell/remote/client.hpp"

namespace UI {

class uiPageSettings;

} // namespace UI

class PageSettings : public QWidget
{
    Q_OBJECT

private:

public:

    PageSettings(QWidget *parent = nullptr);
    ~PageSettings();

private slots:
    void onConnect();
    void onConnected(bool success);

    void onFetch();
private:
    QScopedPointer<UI::uiPageSettings> m_ui;
    yue::bell::remote::RemoteClient m_client;
};

#endif
