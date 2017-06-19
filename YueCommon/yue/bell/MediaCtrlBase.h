#ifndef YUE_BELL_MEDIACTRLBASE_H
#define YUE_BELL_MEDIACTRLBASE_H

#include <QObject>
#include "yue/global.h"
#include "yue/bell/database.hpp"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT MediaCtrlBase : public QObject
{
    Q_OBJECT
public:
    explicit MediaCtrlBase(QObject *parent = nullptr);

signals:
    // signals emitted by the server

    void progressChanged(float progress);



public slots:
    // functions to call (and implement) on the server

    virtual void load(QString path)=0;
    virtual void load(Database::uid_t uid)=0;
    virtual void load()=0;
    virtual void playpause()=0;
    virtual void next()=0;
    virtual void prev()=0;
    virtual void setVolume(float volume) = 0;

    //virtual void setNewPlaylist(SongList lst);

};

} // bell
} // yue


#endif // MEDIACTRLBASE_H
