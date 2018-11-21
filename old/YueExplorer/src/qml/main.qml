import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.yue.common 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    PageExplorer {
        id: explorer
        anchors.fill: parent
    }

}