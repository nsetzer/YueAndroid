
TEMPLATE = subdirs
SUBDIRS = YueCommon YueExplorer YuePlayer Demo \
    Test TagLib \
    YueWidgetTest
Demo.depends = YueCommon
YueExplorer.depends = YueCommon
