
TEMPLATE = subdirs
SUBDIRS = YueCommon YueExplorer Demo \
    Test TagLib
Demo.depends = YueCommon
YueExplorer.depends = YueCommon
