import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
/*
  a basic header which provides a back button and
  centered text indicating the topic of the page
*/
Rectangle {
    id: root
    property font defaultFont

    property string text
    signal backClicked( )

    property bool backVisible: true
    FontMetrics {
        id: fontMetrics
        font: defaultFont
    }

    visible: true

    width: parent.width
    height: currentDirectoryName.height * 2.5

    Rectangle {
        height: 1
        color: "#ccc"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Rectangle {
        height: 1
        color: "#fff"
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.left: parent.left
        anchors.right: parent.right
    }

    gradient: Gradient {
        GradientStop { position: 0 ; color: "#eee" }
        GradientStop { position: 1 ; color: "#ccc" }
    }

    MouseArea{
        anchors.fill: parent
        enabled: true
        //Eats mouse events
    }


    Image {
        id: back
        source: "images/btn_up.png"
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 2
        anchors.left: parent.left
        anchors.leftMargin: 16

        height: parent.height * .75
        width: height

        MouseArea {
            id: mouse
            hoverEnabled: true
            anchors.centerIn: parent
            visible: backVisible
            height: parent.height
            width: height
            anchors.verticalCenterOffset: -1
            onClicked: root.backClicked()
            Rectangle {
                anchors.fill: parent
                opacity: mouse.pressed ? 1 : 0
                Behavior on opacity { NumberAnimation{ duration: 100 }}
                gradient: Gradient {
                    GradientStop { position: 0 ; color: "#22000000" }
                    GradientStop { position: 0.2 ; color: "#11000000" }
                }
                border.color: "darkgray"
                antialiasing: true
                radius: 4
            }
        }
    } //image

    Text {
        id: currentDirectoryName
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: root.text
        font.pointSize: fontMetrics.font.pointSize + 3
    }
}
