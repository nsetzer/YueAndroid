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

class YUECOMMON_EXPORT SongInfo
{
    Q_GADGET
    Q_PROPERTY(QString artist READ artist)
    Q_PROPERTY(QString album READ album)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(Database::uid_t songid READ songid)
public:

    QString artist() { return m_artist; }
    QString album() { return m_album; }
    QString title() { return m_title; }
    Database::uid_t songid() { return m_songid; }

    QString m_artist;
    QString m_title;
    QString m_album;
    Database::uid_t m_songid;
};


class YUECOMMON_EXPORT MediaCtrlBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int currentIndex READ currentIndex  NOTIFY currentIndexChanged)
    Q_PROPERTY(SongInfo currentSong READ currentSong NOTIFY currentIndexChanged)

    static QSharedPointer<yue::bell::MediaCtrlBase> m_instance;
public:
    explicit MediaCtrlBase(QObject *parent = nullptr);

    static void registerInstance(QSharedPointer<MediaCtrlBase> instance) {
        m_instance = instance;
    }

    static QSharedPointer<MediaCtrlBase> instance() {
        return m_instance;
    }

    int currentIndex();
    SongInfo currentSong() { return m_currentSong; }

    Q_INVOKABLE MediaPlayerBase::Status getStatus() { return m_status; }
    Q_INVOKABLE MediaPlayerBase::State getState() { return m_state; }
    Q_INVOKABLE float getProgress() { return m_progress; }

    Q_INVOKABLE virtual void tts() { qDebug() << "tts not implemented"; }

signals:

    void playlistReset(); // the current playlist is changed

    // signals emitted by the server
    void progressChanged(float progress);
    void currentIndexChanged(int index);
    void statusChanged(MediaPlayerBase::Status status);
    void stateChanged(MediaPlayerBase::State state);
    void scanUpdate(bool running, int ndirs, int nfiles, int nsongs);

    void doPlaySong(int uid);
    void doSetCurrentPlaylist(QList<yue::bell::Database::uid_t> lst, bool autoplay);

public slots:
    // functions to call (and implement) on the server

    Q_INVOKABLE void playNext(int uid);
    Q_INVOKABLE void playSong(int uid);
    Q_INVOKABLE void setCurrentPlaylist(QList<yue::bell::Database::uid_t> lst, bool autoplay = false);


    virtual void load()=0;

    virtual void loadIndex(int index)=0;
    virtual void playIndex(int index)=0;
    virtual void playpause()=0;
    virtual void next()=0;
    virtual void prev()=0;
    virtual void sync()=0;
    virtual void setVolume(float volume) = 0;
    virtual void setProgress(float progress) = 0;

    virtual void scanStart(QString path) = 0;
    virtual void scanStop() = 0;
    virtual void scanStatus() = 0;


    //virtual void setNewPlaylist(SongList lst);
private slots:

protected:
    void loadCurrentSongInfo();

    SongInfo m_currentSong;
    float m_progress=0;
    MediaPlayerBase::Status m_status = MediaPlayerBase::Status::Unknown;
    MediaPlayerBase::State m_state = MediaPlayerBase::State::Unknown;



    //virtual void load(Database::uid_t uid)=0;

};

} // bell
} // yue

Q_DECLARE_METATYPE(yue::bell::SongInfo)


#endif // MEDIACTRLBASE_H
