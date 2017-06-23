#ifndef YUE_BELL_MEDIACTRLBASE_H
#define YUE_BELL_MEDIACTRLBASE_H

#include <QObject>
#include <QSharedPointer>
#include "yue/global.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/MediaPlayerBase.h"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT MediaCtrlBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex  NOTIFY currentIndexChanged)

    static QSharedPointer<yue::bell::MediaCtrlBase> m_instance;
public:
    explicit MediaCtrlBase(QObject *parent = nullptr);

    static void registerInstance(QSharedPointer<MediaCtrlBase> instance) {
        m_instance = instance;
    }

    static QSharedPointer<MediaCtrlBase> instance() {
        return m_instance;
    }

    size_t currentIndex();
signals:
    // signals emitted by the server

    void progressChanged(float progress);
    void currentIndexChanged(int index);
    void statusChanged(MediaPlayerBase::Status status);
    void stateChanged(MediaPlayerBase::State state);

public slots:
    // functions to call (and implement) on the server

    virtual void load()=0;

    virtual void loadIndex(int index)=0;
    virtual void playIndex(int index)=0;
    virtual void playpause()=0;
    virtual void next()=0;
    virtual void prev()=0;
    virtual void setVolume(float volume) = 0;
    virtual void setProgress(float progress) = 0;

    //virtual void setNewPlaylist(SongList lst);
private slots:

    //virtual void load(Database::uid_t uid)=0;
};

} // bell
} // yue


#endif // MEDIACTRLBASE_H