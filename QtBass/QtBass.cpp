


#include "QtBass.hpp"
#include "QtBass.impl.hpp"

BassPlayer::BassPlayer(QObject* parent)
    : QObject(parent)
    , m_impl(std::unique_ptr<BassPlayerImpl>(new BassPlayerImpl()))
{
}

BassPlayer::~BassPlayer()
{
}

float BassPlayer::duration() const
{
    return m_impl->duration();
}

float BassPlayer::position() const
{
    return m_impl->position();
}

void BassPlayer::setPosition(float seconds)
{
    m_impl->setPosition(seconds);
}

void BassPlayer::load(QString path)
{
    m_impl->load(path.toUtf8());
}

void BassPlayer::play()
{
    m_impl->play();
}

void BassPlayer::pause()
{
    m_impl->pause();
}

void BassPlayer::playpause()
{
    m_impl->playpause();
}

void BassPlayer::setVolume(float volume)
{
    m_impl->setVolume(volume);
}

void BassPlayer::stop()
{
    m_impl->stop();
}
