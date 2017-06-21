import QtQuick 2.7
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import QtQuick.Controls 2.2

import "qrc:/shared" as SharedQml

import com.yue.common 1.0

ApplicationWindow {
    id: main
    visible: true
    width: 400
    height: 600

    SharedQml.PagePlaylist {
        anchors.fill: parent
    }

}
