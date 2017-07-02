import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.2

import com.yue.common 1.0

Rectangle {
    id: root

    signal clicked();
    property string source: ""

    Image {

        source: root.source
        anchors.centerIn: parent
        width: parent.width*.9
        height: parent.height*.9

    } //image

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: root.clicked();

        Rectangle {
            anchors.fill: parent
            opacity: mouseArea.pressed ? 1 : 0
            Behavior on opacity { NumberAnimation{ duration: 100 }}
            gradient: Gradient {
                GradientStop { position: 0 ; color: "#22000000" }
                GradientStop { position: 0.2 ; color: "#11000000" }
            }
            border.color: "darkgray"
            antialiasing: true
            radius: 4
        }
    } // mouse area
} // rectangle
