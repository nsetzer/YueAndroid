#include "mainwindow.h"






namespace UI {

void TestDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    // https://doc.qt.io/archives/qt-4.8/qstyle.html#StateFlag-enum

    int check = index.data(yue::qtcommon::TreeListModelBase::CheckRole).toInt();
    int expand = index.data(yue::qtcommon::TreeListModelBase::ExpandedRole).toInt();
    int depth = index.data(yue::qtcommon::TreeListModelBase::DepthRole).toInt();
    int count = index.data(yue::qtcommon::TreeListModelBase::ChildCountRole).toInt();

    if (check/*option.state & QStyle::State_Selected*/) {
        painter->fillRect(option.rect, option.palette.highlight());
        painter->setPen(Qt::white);
        painter->setBrush(option.palette.highlightedText());
    }
    else
    {
        painter->setPen(QPen(option.palette.foreground(), 0));
        painter->setBrush(qvariant_cast<QBrush>(index.data(Qt::ForegroundRole)));
    }

    int height = option.rect.height();

    int left = option.rect.left();
    int top = option.rect.top();

    int xoffset=0;
    if (count>0) {
        xoffset+=1;
        int h = static_cast<int>(.75F * height);
        // draw the expansion state
        if (!expand) {
            QRect vrect(left + height/2 - h/2 + (depth*height), top, h/2, h);
            painter->fillRect(vrect, QBrush(QColor(0,0,0)));
        }
        QRect hrect(left + (depth*height), top + height/2 - h/2, h, h/2);
        painter->fillRect(hrect, QBrush(QColor(0,0,0)));
    }

    QRect rect = option.rect;
    rect.setLeft(left + ((depth + xoffset)*height));

    QString text = "(" + index.data(Qt::DisplayRole).toString()
            + ", " + QString::number(check)
            + ", " + QString::number(expand)
            + ", " + QString::number(depth)
            + ", " + QString::number(count) + ")";

    painter->drawText(rect, Qt::AlignVCenter|Qt::AlignLeft, text);
}

LibraryView::LibraryView(QWidget *parent)
  : QListView(parent)
{

    m_delegate = new TestDelegate(this);
    m_model = new yue::qtcommon::LibraryTreeListModel(this);

    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);

    connect(this ,SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onDoubleClick(const QModelIndex)));

    connect(this ,SIGNAL(clicked(const QModelIndex)),
                this, SLOT(onClick(const QModelIndex)));


    m_model->search("");
}


MainWindow::MainWindow(QMainWindow *parent)
    : QObject(parent)
{
    m_centralWidget = new QWidget(parent);
    parent->setCentralWidget(m_centralWidget);

    m_centralLayout = new QVBoxLayout();
    m_centralWidget->setLayout(m_centralLayout);

    m_button = new QPushButton("Click Me", m_centralWidget);


    m_view = new LibraryView(m_centralWidget);

    m_centralLayout->addWidget(m_button);
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
