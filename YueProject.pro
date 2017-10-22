
TEMPLATE = subdirs
SUBDIRS = YueCommon YueExplorer YuePlayer Demo \
    Test TagLib
Demo.depends = YueCommon
YueExplorer.depends = YueCommon
