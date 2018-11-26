
#include "ui/page_settings.h"
#include "yue/qtcommon/gesture.h"

namespace UI {

class uiPageSettings
{

public:

    QVBoxLayout *m_layoutCentral;
    QScrollArea *m_scrollArea;

    QWidget *m_centralWidget;
    QVBoxLayout *m_layoutMain;


    QGroupBox *m_grpPlayer;
    QLabel *m_lblPlayerVolume;
    QSlider *m_sliderPlayerVolume;

    QGroupBox *m_grpRemote;
    QLabel *m_lblRemoteHostname;
    QLabel *m_lblRemoteUsername;
    QLabel *m_lblRemotePassword;
    QLineEdit *m_editRemoteHostname;
    QLineEdit *m_editRemoteUsername;
    QLineEdit *m_editRemotePassword;
    QLabel *m_lblRemoteStatus;
    QPushButton *m_btnConnect;
    QPushButton *m_btnFetch;

    uiPageSettings(QWidget *parent = nullptr);
    ~uiPageSettings();

};

uiPageSettings::uiPageSettings(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_scrollArea = new yue::qtcommon::GestureArea(parent);
    m_centralWidget = new QWidget(parent);
    m_layoutMain = new QVBoxLayout();

    // ------------
    m_grpPlayer = new QGroupBox("Player", parent);
    m_lblPlayerVolume = new QLabel("Volume:", parent);
    m_sliderPlayerVolume = new QSlider(Qt::Orientation::Horizontal, parent);
    m_sliderPlayerVolume->setRange(0, 100);
    {
        QVBoxLayout *vbox = new QVBoxLayout();
        m_grpPlayer->setLayout(vbox);
        vbox->addWidget(m_lblPlayerVolume);
        vbox->addWidget(m_sliderPlayerVolume);
    }

    // ------------
    m_grpRemote = new QGroupBox("Remote", parent);
    m_lblRemoteHostname = new QLabel("Hostname:", parent);
    m_lblRemoteUsername = new QLabel("Username:", parent);
    m_lblRemotePassword = new QLabel("Password:", parent);
    m_editRemoteHostname = new QLineEdit(parent);
    m_editRemoteHostname->setText("http://localhost:4200");
    m_editRemoteUsername = new QLineEdit(parent);
    m_editRemoteUsername->setText("admin");
    m_editRemotePassword = new QLineEdit(parent);
    m_editRemotePassword->setText("admin");
    m_editRemotePassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    m_btnConnect = new QPushButton("Connect", parent);
    m_btnFetch = new QPushButton("Fetch", parent);
    m_lblRemoteStatus = new QLabel("Status:", parent);
    {
        QVBoxLayout *vbox = new QVBoxLayout();
        m_grpRemote->setLayout(vbox);
        vbox->addWidget(m_lblRemoteHostname);
        vbox->addWidget(m_editRemoteHostname);
        vbox->addWidget(m_lblRemoteUsername);
        vbox->addWidget(m_editRemoteUsername);
        vbox->addWidget(m_lblRemotePassword);
        vbox->addWidget(m_editRemotePassword);
        vbox->addWidget(m_btnConnect);

        vbox->addWidget(m_lblRemoteStatus);
        vbox->addWidget(m_btnFetch);
    }
    // -
    parent->setLayout(m_layoutCentral);
    m_layoutCentral->addWidget(m_scrollArea);
    m_centralWidget->setLayout(m_layoutMain);
    //m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->setWidget(m_centralWidget);

    m_layoutMain->addWidget(m_grpPlayer);
    m_layoutMain->addWidget(m_grpRemote);

    m_scrollArea->setWidgetResizable(true);

}

uiPageSettings::~uiPageSettings()
{

}


} // namespace UI

PageSettings::PageSettings(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageSettings(this))
    , m_client()
{

    connect(m_ui->m_btnConnect, &QPushButton::clicked,
            this, &PageSettings::onConnect);

    m_ui->m_btnFetch->setEnabled(false);
    connect(m_ui->m_btnFetch, &QPushButton::clicked,
            this, &PageSettings::onFetch);

    connect(&m_client, &yue::bell::remote::RemoteClient::connected,
            this, &PageSettings::onConnected);
}

PageSettings::~PageSettings() {
}

void PageSettings::onConnect() {

    QString hostname = m_ui->m_editRemoteHostname->text();
    QString username = m_ui->m_editRemoteUsername->text();
    QString password = m_ui->m_editRemotePassword->text();
    m_client.login(hostname, username, password);
}

void PageSettings::onFetch() {

    m_client.fetchLibrary();
}

void PageSettings::onConnected(bool success) {

    qDebug() << "settings connected" <<  success;
    m_ui->m_btnFetch->setEnabled(success);
}
