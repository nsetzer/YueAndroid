import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

import "qrc:/shared" as SharedQml

ApplicationWindow {

    id: main
    visible: true
    width: 400
    height: 600

    //SimpleListDemo {
    //    anchors.fill: parent
   // }

    SharedQml.BasicTreeListDemo {
        anchors.fill: parent
    }

}
