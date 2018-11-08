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

class DynamicQueryEditor : public QFrame {
    Q_OBJECT

    QVBoxLayout *m_layoutCentral;
    QHBoxLayout *m_layoutName;
    QLabel *m_lblName;
    QToolBar *m_barOptions;
    QLineEdit *m_editName;
    QLineEdit *m_editQuery;

    QAction *m_actEdit;
    QAction *m_actRemove;

public:
    DynamicQueryEditor(QWidget *parent = nullptr);
    virtual ~DynamicQueryEditor() override;

    Q_PROPERTY(QString name READ name WRITE setName  NOTIFY nameChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

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

public slots:
    void onToggleEdit();
    void onRemove();

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


    void newEditor();
    void resetEditors();
private:
    QScopedPointer<UI::uiPageDynamic> m_ui;
};

#endif
