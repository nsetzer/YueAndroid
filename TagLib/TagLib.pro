#-------------------------------------------------
#
# taglib-1.11.1
#
#-------------------------------------------------

QT       -= core gui svg

TARGET = tag
TEMPLATE = lib
CONFIG += staticlib

DEFINES += \
   HAVE_ZLIB \
   HAVE_ZLIB_SOURCE \
   VERSION=1.0 \
   SOVERSION=1.0 \
   MAKE_TAGLIB_LIB \
   TAGLIB_STATIC

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os

INCLUDEPATH += taglib\
  taglib/toolkit \
  taglib/asf \
  taglib/mpeg \
  taglib/ogg \
  taglib/ogg/flac \
  taglib/flac \
  taglib/mpc \
  taglib/mp4 \
  taglib/ogg/vorbis \
  taglib/ogg/speex \
  taglib/ogg/opus \
  taglib/mpeg/id3v2 \
  taglib/mpeg/id3v2/frames \
  taglib/mpeg/id3v1 \
  taglib/ape \
  taglib/wavpack \
  taglib/trueaudio \
  taglib/riff \
  taglib/riff/aiff \
  taglib/riff/wav \
  taglib/mod \
  taglib/s3m \
  taglib/it \
  taglib/xm \
  taglib/zlib

SOURCES += \
  taglib/mpeg/mpegfile.cpp \
  taglib/mpeg/mpegproperties.cpp \
  taglib/mpeg/mpegheader.cpp \
  taglib/mpeg/xingheader.cpp \
  taglib/mpeg/id3v1/id3v1tag.cpp \
  taglib/mpeg/id3v1/id3v1genres.cpp \
  taglib/mpeg/id3v2/id3v2framefactory.cpp \
  taglib/mpeg/id3v2/id3v2synchdata.cpp \
  taglib/mpeg/id3v2/id3v2tag.cpp \
  taglib/mpeg/id3v2/id3v2header.cpp \
  taglib/mpeg/id3v2/id3v2frame.cpp \
  taglib/mpeg/id3v2/id3v2footer.cpp \
  taglib/mpeg/id3v2/id3v2extendedheader.cpp \
  taglib/mpeg/id3v2/frames/attachedpictureframe.cpp \
  taglib/mpeg/id3v2/frames/commentsframe.cpp \
  taglib/mpeg/id3v2/frames/eventtimingcodesframe.cpp \
  taglib/mpeg/id3v2/frames/generalencapsulatedobjectframe.cpp \
  taglib/mpeg/id3v2/frames/ownershipframe.cpp \
  taglib/mpeg/id3v2/frames/popularimeterframe.cpp \
  taglib/mpeg/id3v2/frames/privateframe.cpp \
  taglib/mpeg/id3v2/frames/relativevolumeframe.cpp \
  taglib/mpeg/id3v2/frames/synchronizedlyricsframe.cpp \
  taglib/mpeg/id3v2/frames/textidentificationframe.cpp \
  taglib/mpeg/id3v2/frames/uniquefileidentifierframe.cpp \
  taglib/mpeg/id3v2/frames/unknownframe.cpp \
  taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.cpp \
  taglib/mpeg/id3v2/frames/urllinkframe.cpp \
  taglib/mpeg/id3v2/frames/chapterframe.cpp \
  taglib/mpeg/id3v2/frames/tableofcontentsframe.cpp \
  taglib/mpeg/id3v2/frames/podcastframe.cpp \
  taglib/ogg/oggfile.cpp \
  taglib/ogg/oggpage.cpp \
  taglib/ogg/oggpageheader.cpp \
  taglib/ogg/xiphcomment.cpp \
  taglib/ogg/vorbis/vorbisfile.cpp \
  taglib/ogg/vorbis/vorbisproperties.cpp \
  taglib/flac/flacfile.cpp \
  taglib/flac/flacpicture.cpp \
  taglib/flac/flacproperties.cpp \
  taglib/flac/flacmetadatablock.cpp \
  taglib/flac/flacunknownmetadatablock.cpp \
  taglib/ogg/flac/oggflacfile.cpp \
  taglib/mpc/mpcfile.cpp \
  taglib/mpc/mpcproperties.cpp \
  taglib/mp4/mp4file.cpp \
  taglib/mp4/mp4atom.cpp \
  taglib/mp4/mp4tag.cpp \
  taglib/mp4/mp4item.cpp \
  taglib/mp4/mp4properties.cpp \
  taglib/mp4/mp4coverart.cpp \
  taglib/ape/apetag.cpp \
  taglib/ape/apefooter.cpp \
  taglib/ape/apeitem.cpp \
  taglib/ape/apefile.cpp \
  taglib/ape/apeproperties.cpp \
  taglib/wavpack/wavpackfile.cpp \
  taglib/wavpack/wavpackproperties.cpp \
  taglib/ogg/speex/speexfile.cpp \
  taglib/ogg/speex/speexproperties.cpp \
  taglib/ogg/opus/opusfile.cpp \
  taglib/ogg/opus/opusproperties.cpp \
  taglib/trueaudio/trueaudiofile.cpp \
  taglib/trueaudio/trueaudioproperties.cpp \
  taglib/asf/asftag.cpp \
  taglib/asf/asffile.cpp \
  taglib/asf/asfproperties.cpp \
  taglib/asf/asfattribute.cpp \
  taglib/asf/asfpicture.cpp \
  taglib/riff/rifffile.cpp \
  taglib/riff/aiff/aifffile.cpp \
  taglib/riff/aiff/aiffproperties.cpp \
  taglib/riff/wav/wavfile.cpp \
  taglib/riff/wav/wavproperties.cpp \
  taglib/riff/wav/infotag.cpp \
  taglib/mod/modfilebase.cpp \
  taglib/mod/modfile.cpp \
  taglib/mod/modtag.cpp \
  taglib/mod/modproperties.cpp \
  taglib/s3m/s3mfile.cpp \
  taglib/s3m/s3mproperties.cpp \
  taglib/it/itfile.cpp \
  taglib/it/itproperties.cpp \
  taglib/xm/xmfile.cpp \
  taglib/xm/xmproperties.cpp \
  taglib/toolkit/tstring.cpp \
  taglib/toolkit/tstringlist.cpp \
  taglib/toolkit/tbytevector.cpp \
  taglib/toolkit/tbytevectorlist.cpp \
  taglib/toolkit/tbytevectorstream.cpp \
  taglib/toolkit/tiostream.cpp \
  taglib/toolkit/tfile.cpp \
  taglib/toolkit/tfilestream.cpp \
  taglib/toolkit/tdebug.cpp \
  taglib/toolkit/tpropertymap.cpp \
  taglib/toolkit/trefcounter.cpp \
  taglib/toolkit/tdebuglistener.cpp \
  taglib/toolkit/tzlib.cpp \
  taglib/tag.cpp \
  taglib/tagunion.cpp \
  taglib/fileref.cpp \
  taglib/audioproperties.cpp \
  taglib/tagutils.cpp \
  taglib/zlib/adler32.c \
  taglib/zlib/crc32.c \
  taglib/zlib/inffast.c \
  taglib/zlib/inflate.c \
  taglib/zlib/inftrees.c \
  taglib/zlib/zutil.c

HEADERS += \
  taglib/tag.h \
  taglib/fileref.h \
  taglib/audioproperties.h \
  taglib/taglib_export.h \
  taglib/taglib_config.h \
  taglib/toolkit/taglib.h \
  taglib/toolkit/tstring.h \
  taglib/toolkit/tlist.h \
  taglib/toolkit/tlist.tcc \
  taglib/toolkit/tstringlist.h \
  taglib/toolkit/tbytevector.h \
  taglib/toolkit/tbytevectorlist.h \
  taglib/toolkit/tbytevectorstream.h \
  taglib/toolkit/tiostream.h \
  taglib/toolkit/tfile.h \
  taglib/toolkit/tfilestream.h \
  taglib/toolkit/tmap.h \
  taglib/toolkit/tmap.tcc \
  taglib/toolkit/tpropertymap.h \
  taglib/toolkit/trefcounter.h \
  taglib/toolkit/tdebuglistener.h \
  taglib/mpeg/mpegfile.h \
  taglib/mpeg/mpegproperties.h \
  taglib/mpeg/mpegheader.h \
  taglib/mpeg/xingheader.h \
  taglib/mpeg/id3v1/id3v1tag.h \
  taglib/mpeg/id3v1/id3v1genres.h \
  taglib/mpeg/id3v2/id3v2extendedheader.h \
  taglib/mpeg/id3v2/id3v2frame.h \
  taglib/mpeg/id3v2/id3v2header.h \
  taglib/mpeg/id3v2/id3v2synchdata.h \
  taglib/mpeg/id3v2/id3v2footer.h \
  taglib/mpeg/id3v2/id3v2framefactory.h \
  taglib/mpeg/id3v2/id3v2tag.h \
  taglib/mpeg/id3v2/frames/attachedpictureframe.h \
  taglib/mpeg/id3v2/frames/commentsframe.h \
  taglib/mpeg/id3v2/frames/eventtimingcodesframe.h \
  taglib/mpeg/id3v2/frames/generalencapsulatedobjectframe.h \
  taglib/mpeg/id3v2/frames/ownershipframe.h \
  taglib/mpeg/id3v2/frames/popularimeterframe.h \
  taglib/mpeg/id3v2/frames/privateframe.h \
  taglib/mpeg/id3v2/frames/relativevolumeframe.h \
  taglib/mpeg/id3v2/frames/synchronizedlyricsframe.h \
  taglib/mpeg/id3v2/frames/textidentificationframe.h \
  taglib/mpeg/id3v2/frames/uniquefileidentifierframe.h \
  taglib/mpeg/id3v2/frames/unknownframe.h \
  taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.h \
  taglib/mpeg/id3v2/frames/urllinkframe.h \
  taglib/mpeg/id3v2/frames/chapterframe.h \
  taglib/mpeg/id3v2/frames/tableofcontentsframe.h \
  taglib/mpeg/id3v2/frames/podcastframe.h \
  taglib/ogg/oggfile.h \
  taglib/ogg/oggpage.h \
  taglib/ogg/oggpageheader.h \
  taglib/ogg/xiphcomment.h \
  taglib/ogg/vorbis/vorbisfile.h \
  taglib/ogg/vorbis/vorbisproperties.h \
  taglib/ogg/flac/oggflacfile.h \
  taglib/ogg/speex/speexfile.h \
  taglib/ogg/speex/speexproperties.h \
  taglib/ogg/opus/opusfile.h \
  taglib/ogg/opus/opusproperties.h \
  taglib/flac/flacfile.h \
  taglib/flac/flacpicture.h \
  taglib/flac/flacproperties.h \
  taglib/flac/flacmetadatablock.h \
  taglib/ape/apefile.h \
  taglib/ape/apeproperties.h \
  taglib/ape/apetag.h \
  taglib/ape/apefooter.h \
  taglib/ape/apeitem.h \
  taglib/mpc/mpcfile.h \
  taglib/mpc/mpcproperties.h \
  taglib/wavpack/wavpackfile.h \
  taglib/wavpack/wavpackproperties.h \
  taglib/trueaudio/trueaudiofile.h \
  taglib/trueaudio/trueaudioproperties.h \
  taglib/riff/rifffile.h \
  taglib/riff/aiff/aifffile.h \
  taglib/riff/aiff/aiffproperties.h \
  taglib/riff/wav/wavfile.h \
  taglib/riff/wav/wavproperties.h \
  taglib/riff/wav/infotag.h \
  taglib/asf/asffile.h \
  taglib/asf/asfproperties.h \
  taglib/asf/asftag.h \
  taglib/asf/asfattribute.h \
  taglib/asf/asfpicture.h \
  taglib/mp4/mp4file.h \
  taglib/mp4/mp4atom.h \
  taglib/mp4/mp4tag.h \
  taglib/mp4/mp4item.h \
  taglib/mp4/mp4properties.h \
  taglib/mp4/mp4coverart.h \
  taglib/mod/modfilebase.h \
  taglib/mod/modfile.h \
  taglib/mod/modtag.h \
  taglib/mod/modproperties.h \
  taglib/it/itfile.h \
  taglib/it/itproperties.h \
  taglib/s3m/s3mfile.h \
  taglib/s3m/s3mproperties.h \
  taglib/xm/xmfile.h \
  taglib/xm/xmproperties.h \
  taglib/zlib/zlib.h

unix {
    SOURCES += taglib/toolkit/unicode.cpp
    target.path = /usr/lib
    INSTALLS += target
}
