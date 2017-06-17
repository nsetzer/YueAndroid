import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2

import com.yue.common 1.0

/*
  use a one shot timer that re-arms itself
  two regions, 1.5* the height of a row +/- a margin so that the center
    of mass is in the region ....
  when in the region add or subtract the current index to scroll the view.


  Qt.darker("tomato", 1.1) : "tomato"
  */

Rectangle {
    id: root

    width: 300; height: 400

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


    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false

            anchors { left: parent.left; right: parent.right }
            height: content.height

            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis

            onPressAndHold: held = true
            onReleased: held = false

            Rectangle {
                id: content
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                width: dragArea.width; height: column.implicitHeight * 3

                border.width: 1
                border.color: "lightsteelblue"

                color: dragArea.held ? "lightsteelblue" : "white"
                Behavior on color { ColorAnimation { duration: 100 } }

                radius: 2
                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
                states: State {
                    when: dragArea.held

                    ParentChange { target: content; parent: root }
                    AnchorChanges {
                        target: content
                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
                    }
                }

                Column {
                    id: column
                    anchors { fill: parent; margins: 2 }

                    Text { text: 'Size: ' + title }
                }
            }
            DropArea {
                anchors { fill: parent; margins: 10 }

                onEntered: {
                    visualModel.items.move(
                            drag.source.DelegateModel.itemsIndex,
                            dragArea.DelegateModel.itemsIndex)
                }
            }
        }
    }

    DelegateModel {
        id: visualModel

        model: listModel
        delegate: dragDelegate
    }

    ListView {
        id: view

        anchors { fill: parent; margins: 2 }

        model: visualModel

        spacing: 4
        cacheBuffer: 50

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 250; easing: Easing.OutQuad }
        }
    }
}

