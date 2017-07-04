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

    // make sure x is odd
    function checkOdd(x) {
        if (x%2==0)
            return x+1
        return x
    }

    property var primaryColor: "black"
    property var secondaryColor: "blue"

    property bool expanded: false
    property int itemCheckState: Qt.Unchecked

    signal clicked();
    color: "transparent"


    property real rectLength: root.height * .8
    property real rectWidth: root.height * .2

    // -I-

    // I
    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        radius: rectWidth/3
        //visible: !root.expanded
        color: (itemCheckState == Qt.Unchecked) ? root.primaryColor : root.secondaryColor
        width: rectWidth
        height: (!root.expanded) ? rectLength : 0

        Behavior on height {
            NumberAnimation { duration: 250 }
        }
    }

    // --
    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        radius: rectWidth/3
        color: (itemCheckState == Qt.Unchecked) ? root.primaryColor : root.secondaryColor
        width: rectLength
        height: rectWidth

    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.clicked();
    }
}
