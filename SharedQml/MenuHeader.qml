import QtQuick 2.7
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import QtQuick.Controls 2.2

Rectangle {
    id: root
    property string title: ""

    signal clicked();

    MouseArea {
        anchors.fill: parent

        onClicked: {
            root.clicked();
        }
    }

    Rectangle {
        id: menuButton
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 1.2*height
        height: parent.height
        scale: maMenuBar.pressed ? 1.2 : 1
        color: "transparent"

        MenuIconLive {
            id: menuBackIcon
            scale: (parent.height/height)*0.65
            anchors.centerIn: parent
            value: menuProgressOpening
        }
        MouseArea {
            id: maMenuBar
            anchors.fill: parent
            onClicked: onMenu()
        }
        clip: true
    }
    Label {
        id: titleText
        anchors.left: menuButton.right
        anchors.verticalCenter: parent.verticalCenter
        text: title
        font.pixelSize: 0.35*parent.height
        color: palette.text
    }


}
