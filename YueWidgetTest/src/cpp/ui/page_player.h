#ifndef UI_PAGE_PLAYER_H
#define UI_PAGE_PLAYER_H

#include <QScopedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QProgressBar>
#include <QLabel>

#include "yue/bell/MediaPlayerBase.h"

namespace UI {

class uiPagePlayer;

} // namespace UI

class PagePlayer : public QWidget
{
    Q_OBJECT

public:

    PagePlayer(QWidget *parent = nullptr);
    ~PagePlayer();

protected:

    void onPlayPauseClicked();
    void onNextSongClicked();
    void onProgressChanged(float progress);
    void onCurrentIndexChanged(int index=-1);
    void onStatusChanged(yue::bell::MediaPlayerBase::Status status);
    void onStateChanged(yue::bell::MediaPlayerBase::State state);

private:
    QScopedPointer<UI::uiPagePlayer> m_ui;
    yue::bell::MediaPlayerBase::Status m_status;
};

#endif
