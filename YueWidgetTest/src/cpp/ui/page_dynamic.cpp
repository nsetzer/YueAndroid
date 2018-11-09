
#include "ui/page_dynamic.h"
#include "yue/bell/settings.h"

#include <QApplication>

DynamicQueryEditor::DynamicQueryEditor(int index, QWidget *parent)
    : QFrame(parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_layoutName = new QHBoxLayout();

    m_lblName = new QLabel(this);
    m_lblIndex = new QLabel(this);

    m_barOptions = new yue::qtcommon::ToolBar(this);
    m_editName = new QLineEdit(this);
    m_editQuery = new QLineEdit(this);

    // order here is chosen so that the edit button does not move
    // when the editing mode is toggeled. this allows for accidental double taps
    m_actRemove = m_barOptions->addAction(QIcon(":/res/delete.svg"), "");
    m_actEdit = m_barOptions->addAction(QIcon(":/res/edit.svg"), "");
    m_actCreate = m_barOptions->addAction(QIcon(":/res/playlist.svg"), "");

    // --
    this->setLayout(m_layoutCentral);
    m_layoutCentral->addLayout(m_layoutName);
    m_layoutCentral->addWidget(m_editQuery);
    m_layoutName->addWidget(m_lblIndex);
    m_layoutName->addWidget(m_lblName);
    m_layoutName->addWidget(m_editName);
    m_layoutName->addWidget(m_barOptions);

    // --

    m_editName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_barOptions->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_editName->setPlaceholderText("Edit Name");
    m_editQuery->setPlaceholderText("Blank Search");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(2);

    connect(m_actEdit, &QAction::triggered,
            this, &DynamicQueryEditor::onToggleEdit);

    connect(m_actRemove, &QAction::triggered,
            this, &DynamicQueryEditor::onRemove);

    connect(m_actCreate, &QAction::triggered,
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
    return m_editName->text();
}

QString DynamicQueryEditor::query() const {
    return m_editQuery->text();
}

void DynamicQueryEditor::setName(QString name) {
    m_lblName->setText(name);
    m_editName->setText(name);
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
    m_actEdit->setIcon(QIcon((enabled)?":/res/edit_finished.svg":":/res/edit.svg"));
    m_actRemove->setVisible(enabled);
    m_editQuery->setVisible(enabled);
    qDebug() << enabled;

    if(!enabled) {
        m_lblName->setText(m_editName->text());
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

    yue::qtcommon::ToolBar *m_barActions;

    uiPageDynamic(QWidget *parent = nullptr);
    ~uiPageDynamic();

};

uiPageDynamic::uiPageDynamic(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_scrollArea = new QScrollArea(parent);
    m_centralWidget = new QWidget(parent);
    m_layoutMain = new QVBoxLayout();

    m_barActions = new yue::qtcommon::ToolBar(2.0, parent);

    // -
    parent->setLayout(m_layoutCentral);
    m_layoutCentral->addWidget(m_scrollArea);

    m_layoutCentral->addWidget(m_barActions);
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
    auto action = m_ui->m_barActions->addAction(QIcon(":/res/file.svg"), "");
    connect(action, &QAction::triggered,
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

    for (const auto& pair : yue::bell::Settings::instance()->dynamicLists()) {
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
