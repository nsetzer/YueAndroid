import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    //http://www.bytebau.com/progress-circle-with-qml-and-javascript/
    function min(x,y) {
        return (x < y) ? x : y
    }
    function max(x,y) {
        return (x > y) ? x : y
    }

    property bool expanded: false
    property int itemCheckState: Qt.Unchecked

    signal clicked();
    color: "transparent"


    property real rectMargin: root.height * 0.1
    property real rectThickness: (root.height - 2 * rectMargin) / 5


    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        //visible: !root.expanded
        color: (itemCheckState == Qt.Unchecked) ? "black" : "blue"
        width: root.rectThickness
        height: (!root.expanded) ? root.height - 2 * rectMargin : 0

        Behavior on height {
            NumberAnimation { duration: 250 }
        }
    }

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        color: (itemCheckState == Qt.Unchecked) ? "black" : "blue"
        width: root.height - 2 * rectMargin
        height: root.rectThickness

    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.clicked();
    }
}
