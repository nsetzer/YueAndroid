
#ifndef QTBASS_HPP
#define QTBASS_HPP

#if defined(YUECOMMON_LIBRARY)
#  define QTBASS_EXPORT Q_DECL_EXPORT
#else
#  define QTBASS_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <string>
#include <memory>
#include <exception>
#include <stdexcept>


class BassPlayerError : public std::runtime_error
{
public:
    BassPlayerError(std::string msg)
        : std::runtime_error(msg)
    {}
    virtual ~BassPlayerError() = default;

};

class BassPlayerImpl;

class QTBASS_EXPORT BassPlayer : public QObject
{
    Q_OBJECT
    std::unique_ptr<BassPlayerImpl> m_impl;
public:

    enum class Status {
        Unknown=0,
        Error=1,    // error is preventing playback
        Loading=2,  // media is not ready to play
        Ready=3,    // play() will begin playback
        Ended=4,    // media has finished playing
    };

    enum class State {
        Unknown=0,
        Playing=1,
        Paused=2,
    };


    BassPlayer(QObject* parent=nullptr);
    virtual ~BassPlayer();

    float duration() const;
    float position() const;
    void setPosition(float seconds);

signals:

    void progressChanged(float progress); // as float, 0.0 to 1.0
    void statusChanged(Status status);
    void stateChanged(State state);

public slots:

    void load(QString path);
    void play();
    void pause();
    void playpause();
    void setVolume(float volume);
    void stop();

};

#endif //QTBASS_HPP
