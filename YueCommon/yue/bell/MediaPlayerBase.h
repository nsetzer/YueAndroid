#ifndef YUE_BELL_MEDIAPLAYERBASE_H
#define YUE_BELL_MEDIAPLAYERBASE_H

#include <QObject>
#include "yue/global.h"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT MediaPlayerBase : public QObject
{
    Q_OBJECT

public:
    explicit MediaPlayerBase(QObject *parent = nullptr);

signals:

    void progressChanged(float progress); // as float, 0.0 to 1.0

public slots:

    virtual void load(QString path)=0;
    virtual void playpause()=0;
    //virtual void next()=0;
    //virtual void prev()=0;
    virtual void setVolume(float volume) = 0;

};

} // bell
} // yue

#endif // MEDIAPLAYER_H
