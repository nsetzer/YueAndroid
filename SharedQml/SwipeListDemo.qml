import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2

import com.yue.common 1.0

PageBase {

    id: listDemo
    title: "List Demo"

    function openPage() {
        console.log("opening page: " + title);
        //plmodel.playlistName = ":current:"
    }

    Device {id:device}

    ListModel {
        id: listModel

        ListElement {  title: "A 123" }
        ListElement {  title: "A 456" }
        ListElement {  title: "A 789" }
        ListElement {  title: "B 123" }
        ListElement {  title: "B 456" }
        ListElement {  title: "B 789" }
        ListElement {  title: "C 123" }
        ListElement {  title: "C 456" }
        ListElement {  title: "C 789" }
        ListElement {  title: "D 123" }
        ListElement {  title: "D 456" }
        ListElement {  title: "D 789" }
        ListElement {  title: "E 123" }
        ListElement {  title: "E 456" }
        ListElement {  title: "E 789" }
        ListElement {  title: "F 123" }
        ListElement {  title: "F 456" }
        ListElement {  title: "F 789" }
    }
    Component{
        id: swipeComponent
    SwipeDelegate {
        id: swipeDelegate
        width: parent.width

        Rectangle {
            id: swipeContent
            anchors.fill: parent
            z: -1
            Text {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.horizontalCenter
                text: title
            }
            Text {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.horizontalCenter
                text: title
            }
        }

        ListView.onRemove: SequentialAnimation {
            PropertyAction {
                target: swipeDelegate
                property: "ListView.delayRemove"
                value: true
            }
            NumberAnimation {
                target: swipeDelegate
                property: "height"
                to: 0
                easing.type: Easing.InOutQuad
            }
            PropertyAction {
                target: swipeDelegate;
                property: "ListView.delayRemove";
                value: false
            }
        }

        swipe.behind: Label {
            id: deleteLabel
            text: qsTr("Delete")
            color: "white"
            verticalAlignment: Label.AlignVCenter
            padding: 12
            height: parent.height
            width: parent.width
            anchors.right: parent.right

            SwipeDelegate.onClicked: listModel.remove(index)

            background: Rectangle {
                color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
            }
        }
    }
    }

    ListView {
        id: myList
        anchors.fill: parent
        clip: true
        maximumFlickVelocity: 4000*device.dp

        model: listModel
        delegate: swipeComponent
        ScrollBar.vertical: ScrollBar { }

        /*
        remove: Transition {
            NumberAnimation { property: "opacity"; to:0; duration:250; }
        }
        removeDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration:250; }
        }
        */
    } //listview

}
