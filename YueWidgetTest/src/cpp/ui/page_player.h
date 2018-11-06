#ifndef UI_PAGE_PLAYER_H
#define UI_PAGE_PLAYER_H

#include <QScopedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QProgressBar>
#include <QLabel>

namespace UI {

class uiPagePlayer;

} // namespace UI

class PagePlayer : public QWidget
{
    Q_OBJECT

private:

public:

    PagePlayer(QWidget *parent = nullptr);
    ~PagePlayer();

protected:

    void onPlayPauseClicked(bool checked=false);

private:
    QScopedPointer<UI::uiPagePlayer> m_ui;
};

#endif
