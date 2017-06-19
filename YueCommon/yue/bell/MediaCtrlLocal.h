#ifndef YUE_BELL_MEDIACTRLLOCAL_H
#define YUE_BELL_MEDIACTRLLOCAL_H


#include "yue/global.h"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/bell/MediaPlayerBase.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"

namespace yue {
namespace bell {


class YUECOMMON_EXPORT MediaCtrlLocal : public MediaCtrlBase
{
    MediaPlayerBase* m_pPlayer;
public:
    MediaCtrlLocal(QObject *parent = nullptr);
    ~MediaCtrlLocal();

signals:
    //void progressChanged(float progress);

public slots:
    virtual void load(QString path);
    virtual void load(Database::uid_t);
    virtual void load();
    virtual void playpause();
    virtual void next();
    virtual void prev();
    virtual void setVolume(float volume);

private slots:
    void onProgressChanged(float progress);
};

} // bell
} // yue

#endif // MEDIACTRLLOCAL_H
