
#include "ui/page_dynamic.h"
#include "yue/bell/settings.h"

#include <QApplication>

DynamicQueryEditor::DynamicQueryEditor(int index, QWidget *parent)
    : QFrame(parent)
    , m_name()
{
    m_layoutCentral = new QVBoxLayout();
    m_layoutName = new QHBoxLayout();

    m_lblName = new QLabel(this);
    m_lblIndex = new QLabel(this);

    m_editName = new QLineEdit(this);
    m_editQuery = new QLineEdit(this);

    // order here is chosen so that the edit button does not move
    // when the editing mode is toggeled. this allows for accidental double taps
    m_btnRemove = new yue::qtcommon::IconButton(QIcon(":/res/delete.svg"), this);
    m_btnEdit = new yue::qtcommon::IconButton(QIcon(":/res/edit.svg"), this);
    m_btnCreate = new yue::qtcommon::IconButton(QIcon(":/res/playlist.svg"), this);

    // --
    this->setLayout(m_layoutCentral);
    m_layoutCentral->addLayout(m_layoutName);
    m_layoutCentral->addWidget(m_editQuery);
    m_layoutName->addWidget(m_lblIndex);
    m_layoutName->addWidget(m_lblName);
    m_layoutName->addWidget(m_editName);
    m_layoutName->addWidget(m_btnRemove);
    m_layoutName->addWidget(m_btnCreate);
    m_layoutName->addWidget(m_btnEdit);

    // --

    m_editName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_editName->setPlaceholderText("Edit Name");
    m_editQuery->setPlaceholderText("Edit Query");

    m_lblName->setText("New Dynamic List");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(2);

    connect(m_btnEdit, &yue::qtcommon::IconButton::clicked,
            this, &DynamicQueryEditor::onToggleEdit);

    connect(m_btnRemove, &yue::qtcommon::IconButton::clicked,
            this, &DynamicQueryEditor::onRemove);

    connect(m_btnCreate, &yue::qtcommon::IconButton::clicked,
            this, &DynamicQueryEditor::onCreate);

    // --

    enableEditing(false);

    if (index >= 0) {
        m_lblIndex->setText(QString::number(index) + ":");
    }

    // --

}

DynamicQueryEditor::~DynamicQueryEditor()
{

}

QString DynamicQueryEditor::name() const {
    return m_name;
}

QString DynamicQueryEditor::query() const {
    return m_editQuery->text();
}

void DynamicQueryEditor::setName(QString name) {
    m_name = name;

    if (name.isEmpty()) {
        m_lblName->setText("New Dynamic List");
        m_editName->setText("");
    } else {
        m_lblName->setText(name);
        m_editName->setText(name);
    }
    emit nameChanged();
}
void DynamicQueryEditor::setQuery(QString query) {
    m_editQuery->setText(query);
    emit queryChanged();
}

void DynamicQueryEditor::enableEditing(bool enabled)
{

    m_editName->setVisible(enabled);
    m_lblName->setVisible(!enabled);
    m_btnEdit->setIcon(QIcon((enabled)?":/res/edit_finished.svg":":/res/edit.svg"));
    m_btnRemove->setVisible(enabled);
    m_btnCreate->setVisible(!enabled);
    m_editQuery->setVisible(enabled);
    qDebug() << enabled;

    if(!enabled) {

        m_name = m_editName->text();
        m_lblName->setText(m_name.isEmpty()?"New Dynamic List":m_name);
        emit editingFinished();
    }
}

bool DynamicQueryEditor::editing() const
{
    return m_editName->isVisible();
}

void DynamicQueryEditor::onToggleEdit()
{
    enableEditing(!editing());
}

void DynamicQueryEditor::onRemove()
{
    emit remove();
}

void DynamicQueryEditor::onCreate()
{
    emit create(query());
}


namespace UI {

class uiPageDynamic
{

public:


    QVBoxLayout *m_layoutCentral;
    QScrollArea *m_scrollArea;

    QWidget *m_centralWidget;
    QVBoxLayout *m_layoutMain;

    yue::qtcommon::IconButton *m_btnNewEditor;

    uiPageDynamic(QWidget *parent = nullptr);
    virtual ~uiPageDynamic();



};

uiPageDynamic::uiPageDynamic(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_scrollArea = new yue::qtcommon::GestureArea(parent);
    m_centralWidget = new QWidget(parent);
    m_layoutMain = new QVBoxLayout();

    m_btnNewEditor = new yue::qtcommon::IconButton(QIcon(":/res/file.svg"), parent);


    // -
    parent->setLayout(m_layoutCentral);
    m_layoutCentral->addWidget(m_scrollArea);

    m_layoutCentral->addWidget(m_btnNewEditor);
    m_centralWidget->setLayout(m_layoutMain);
    //m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->setWidget(m_centralWidget);
    m_scrollArea->setWidgetResizable(true);


    m_layoutMain->addStretch(1);
}

uiPageDynamic::~uiPageDynamic()
{

}


} // namespace UI

PageDynamic::PageDynamic(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageDynamic(this))
{

    connect(m_ui->m_btnNewEditor, &yue::qtcommon::IconButton::clicked,
            this, &PageDynamic::newEditor);

    resetEditors();
}

PageDynamic::~PageDynamic() {
}

void PageDynamic::newEditor()
{
    createEditor("", "");
}

void PageDynamic::createEditor(QString name, QString query)
{
    int idx = m_ui->m_layoutMain->count() - 1;
    auto item = new DynamicQueryEditor(1 + idx, this);
    item->setName(name);
    item->setQuery(query);
    m_ui->m_layoutMain->insertWidget(idx, item);

    connect(item, &DynamicQueryEditor::remove,
            [=](){
        m_ui->m_layoutMain->removeWidget(item);
        // cannot delete widgets which have active signals/slots connected
        // hide and delete later
        item->hide();
        item->setParent(nullptr);
        item->deleteLater();
    });

    connect(item, &DynamicQueryEditor::editingFinished,
            this, &PageDynamic::onEditingFinished);

    connect(item, &DynamicQueryEditor::create,
            this, &PageDynamic::onCreate);
}

void PageDynamic::resetEditors()
{

    qDebug() << "reseting all editors";

    // the last item is always a spacer, which we want to keep
    while (m_ui->m_layoutMain->count() > 1) {
        // TODO i expect this to crash
        auto child = m_ui->m_layoutMain->takeAt(0);
        delete child;
    }

    QList<QPair<QString, QString>> lst = yue::bell::Settings::instance()->dynamicLists();
    qSort(lst.begin(), lst.end(), [](const QPair<QString, QString>& a,const QPair<QString, QString>& b) -> bool {
        return a.first < b.first;
    });
    for (const auto& pair : lst) {
        createEditor(pair.first, pair.second);
    }
}

void PageDynamic::onEditingFinished() {

    QList<QPair<QString, QString>> lst;
    for (int i=0; i < m_ui->m_layoutMain->count()-1; i++) {
        auto item = m_ui->m_layoutMain->itemAt(i);

        if (DynamicQueryEditor* edit = qobject_cast<DynamicQueryEditor*>(item->widget())) {
            QString name = edit->name();
            QString query = edit->query();
            if (!name.isEmpty()) {
                lst.append(QPair<QString, QString>(name, query));
            }
        }
    }

    yue::bell::Settings::instance()->setDynamicLists(lst);

}

void PageDynamic::onCreate(QString query) {
    emit search(query);
}
