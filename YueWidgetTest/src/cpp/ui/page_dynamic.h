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
#include <QToolBar>
#include <QListWidget>
#include <QResizeEvent>
#include <QDebug>

#include "yue/qtcommon/iconbutton.h"
#include "yue/qtcommon/gesture.h"

class DynamicQueryEditor : public QFrame {
    Q_OBJECT

    QVBoxLayout *m_layoutCentral;
    QHBoxLayout *m_layoutName;
    QLabel *m_lblName;
    QLabel *m_lblIndex;
    QLineEdit *m_editName;
    QLineEdit *m_editQuery;

    QString m_name;

    yue::qtcommon::IconButton *m_btnEdit;
    yue::qtcommon::IconButton *m_btnRemove;
    yue::qtcommon::IconButton *m_btnCreate;

public:
    DynamicQueryEditor(int index=-1, QWidget *parent = nullptr);
    virtual ~DynamicQueryEditor() override;

    QString name() const;
    QString query() const;

    void setName(QString name);
    void setQuery(QString query);

    void enableEditing(bool enabled);
    bool editing() const;

signals:
    void nameChanged();
    void queryChanged();
    void remove();
    void create(QString query);
    void editingFinished();

public slots:
    void onToggleEdit();
    void onRemove();
    void onCreate();


private:

};

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

signals:
    void search(QString query);

public slots:
    void newEditor();
    void resetEditors();
    void onEditingFinished();
    void onCreate(QString query);

private:
    QScopedPointer<UI::uiPageDynamic> m_ui;

    void createEditor(QString name, QString query);
};

#endif
