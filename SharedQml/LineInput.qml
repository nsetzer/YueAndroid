import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQml.Models 2.2

import com.yue.common 1.0

FocusScope {
    id: wrapper

    property alias text: input.text
    property alias hint: hint.text
    property alias prefix: prefix.text

    signal accepted

    Rectangle {
        anchors.fill: parent
        border.color: "#707070"
        //color: "#c1c1c1"
        radius: 4
        height: gDevice.textHeight * 1.5

        Text {
            id: hint
            anchors { fill: parent; leftMargin: 14 }
            verticalAlignment: Text.AlignVCenter
            text: "Enter word"
            //font.pixelSize: 18
            //color: "#707070"
            opacity: input.displayText.length ? 0 : 1
        }

        Text {
            id: prefix
            anchors { left: parent.left; leftMargin: 14; verticalCenter: parent.verticalCenter }
            verticalAlignment: Text.AlignVCenter
            //font.pixelSize: 18
            //color: "#707070"
            opacity: !hint.opacity
        }

        TextInput {
            id: input
            focus: true
            anchors { left: prefix.right; right: parent.right; top: parent.top; bottom: parent.bottom }
            verticalAlignment: Text.AlignVCenter
            //font.pixelSize: 18
            //color: "#707070"
            onAccepted: wrapper.accepted()
        }

        Image {
            source: "qrc:/shared/images/00_search_b.svg"
            anchors.right: parent.right
            anchors.rightMargin: width/2
            anchors.verticalCenter: parent.verticalCenter
            asynchronous: true
            sourceSize.height: parent.height
            MouseArea {
                onClicked: wrapper.accepted()
            }
        }
    }
}
