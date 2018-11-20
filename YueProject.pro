
TEMPLATE = subdirs
SUBDIRS = YuePlayer TagLib

YuePlayer.depends = TagLib
# YueTest.depends = TagLib


#####################################################################
# hack to build arm64-v8a on windows
#
# configure the sdk/ndk to use:
#   C:\Users\nicks\AppData\Local\Android\Sdk
#   C:\Qt-ndk\android-ndk-r16b
#
# copy all from here:
#   C:\Qt-ndk\android-ndk-r16b\platforms\android-23\arch-arm64\usr\lib
# into here:
#   C:\Qt-ndk\android-ndk-r16b\sources\cxx-stl\llvm-libc++\libs\arm64-v8a
# *** the linker has this second path by default
#
# copy crtbegin_so.o, crtend_so.o from here:
#   C:\Qt-ndk\android-ndk-r16b\platforms\android-23\arch-arm64\usr\lib
# to here:
#   D:\git\build-YueProject-Android_for_arm64_v8a_GCC_4_9_Qt_5_12_0_for_Android_ARM64_v8a-Debug\YuePlayer
# for some reason it ignores $$NDK_ROOT\platforms\android-23\arch-arm64\usr\lib
# but will look in the current working directory
# configure a post-qmake pre-make build step to copy these files
#
# I figured this out by  first deleting:
#   * YueProject.pro.user
#   * all project build directories
#   * %APPDATA%\QtProject
#   * %APPDATA%\..\Local\QtProject
#
# Then opening Qt, reconfiguring the kits, rebuilding the project
# and using the compile output to look at the failing linking command
