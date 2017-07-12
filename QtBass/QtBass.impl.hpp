#ifndef QTBASS_IMPL_HPP
#define QTBASS_IMPL_HPP

#include <map>
#include <stdio.h>
#include <stdlib.h>

#include "QtBass.hpp"
#include "bass.h"

std::map<int,std::string> gBassErrorMap({
    {BASS_OK,"all is OK"},
});



std::string get_error_description(int error)
{
    if (gBassErrorMap.count(error) > 0) {
        return gBassErrorMap[error];
    }
    char buffer[16];
    sprintf(buffer,"%d",error);
    return std::string("unkown BASS error code ") + std::string(buffer);
}

class BassPlayerImpl
{
public:
    BassPlayerImpl() {}
    virtual ~BassPlayerImpl() {}

    static void init(int sampleRate=44100) {
        BASS_SetConfig(BASS_CONFIG_FLOATDSP,true);
        BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT,true);

        if (!BASS_Init(-1, sampleRate, 0, 0, 0)) {
            int error = BASS_ErrorGetCode();
            throw BassPlayerError(get_error_description(error));
        }
    }

    float duration() const {

    }
    float position() const {

    }
    void setPosition(float seconds) {

    }


signals:

    void progressChanged(float progress);
    void statusChanged(BassPlayer::Status status);
    void stateChanged(BassPlayer::State state);

public slots:

    void load(const char* path) {
        int flags = BASS_STREAM_AUTOFREE;
#ifdef Q_OS_WIN32
        flags |= BASS_UNICODE;
#endif

        HSTREAM channel = BASS_StreamCreateFile(false, path, 0, 0, flags);
        int error = BASS_ErrorGetCode();

        if (channel == 0 || error != 0) {
            throw BassPlayerError(get_error_description(error));
        }
    }
    void play() {

    }
    void pause() {

    }
    void playpause() {

    }
    void setVolume(float volume) {

    }
    void stop() {

    }
};

#endif // QTBASS_IMPL_HPP
