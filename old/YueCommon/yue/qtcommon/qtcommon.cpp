
#include "yue/global.h"
#include "yue/qtcommon/qtcommon.hpp"

#include "yue/qtcommon/LibraryTreeListModel.hpp"
#include "yue/qtcommon/TreeListModelBase.hpp"
#include "yue/qtcommon/examples/BasicListModel.hpp"
#include "yue/qtcommon/examples/SwipeListModel.hpp"
#include "yue/qtcommon/DirectoryListModel.hpp"
#include "yue/qtcommon/PlaylistModel.h"
#include "yue/qtcommon/SongListModel.h"
#include "yue/qtcommon/AlbumArtImage.h"
#include "yue/device.h"


namespace yue {
namespace qtcommon {

YUECOMMON_EXPORT void registerQmlTypes()
{
    qmlRegisterType<BasicListModel>("com.yue.common", 1, 0, "BasicListModel");
    qmlRegisterType<SwipeListModel>("com.yue.common", 1, 0, "SwipeListModel");
    qmlRegisterType<TreeListModelBase>("com.yue.common", 1, 0, "TreeListModelBase");
    qmlRegisterType<LibraryTreeListModel>("com.yue.common", 1, 0, "LibraryTreeListModel");
    qmlRegisterType<DirectoryListModel>("com.yue.common", 1, 0, "DirectoryListModel");
    qmlRegisterType<PlaylistModel>("com.yue.common", 1, 0, "PlaylistModel");
    qmlRegisterType<SongListModel>("com.yue.common", 1, 0, "SongListModel");
    qmlRegisterType<AlbumArtImage>("com.yue.common", 1, 0, "AlbumArtImage");
    //qmlRegisterType<Device>("com.yue.common", 1, 0, "Device");
}

} // qtcommon
} // yue
