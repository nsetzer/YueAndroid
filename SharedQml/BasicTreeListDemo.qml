import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2

import com.yue.common 1.0

/*

  TODO:

  onRemove
    animation
    index is not updating, try using a real model
    instead of the qml model...

  Two regions, 1.5* height of a row +/- some margin
    when draging, and inside this region start a timer
    that, when fired changes the current index +/- 1 of the list view.
    this will scroll the region automatically

  Qt.darker("tomato", 1.1) : "tomato"
  */

Rectangle {
    id: root

    width: 300; height: 400

    LibraryTreeListModel {
        id: treeModel
    }


    ListView {
        id: view
        anchors.fill: parent
        model: treeModel
        delegate:  Rectangle {
            id: delegateItem
            height: targetHeight
            anchors.left: parent.left
            anchors.right: parent.right

            readonly property int targetHeight: delegateText.implicitHeight * 3

            ExpanderButton {
                id: delegateButton
                //text: "clickme"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: depth * (height/2)

                visible: childCount > 0
                expanded: isExpanded
                itemCheckState: checkState

                width: height

                onClicked: {
                    treeModel.toggleExpandItem( index )
                }
            }

            Text {
                id: delegateText
                anchors.verticalCenter: parent.verticalCenter

                anchors.right: parent.right
                anchors.left: delegateButton.right
                text: display
            }

            MouseArea {
                anchors.left: delegateButton.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                onClicked: treeModel.toggleCheckState( index )

            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                visible: checkState == Qt.Checked
                color: "#220000FF"
            }

            ListView.onAdd: SequentialAnimation {
                NumberAnimation {
                    target: delegateItem;
                    property: "height";
                    from: 0;
                    to: delegateItem.targetHeight;
                    duration: 250;
                    easing.type: Easing.OutQuad;
                }
            }

            /*
            ListView.onRemove: SequentialAnimation {
                PropertyAction { target: delegateItem; property: "ListView.delayRemove"; value: true }
                NumberAnimation {
                    target: delegateItem;
                    property: "height";
                    from: delegateItem.targetHeight;
                    to: 0;
                    duration: 250;
                    easing.type: Easing.OutQuad;
                }
                PropertyAction { target: delegateItem; property: "ListView.delayRemove"; value: false }
            }
            */

            Rectangle {
                anchors.bottom: parent.bottom;
                height: 2
                width: parent.width
                color: "#33777777"
            }

        }

        addDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 500 }
        }

        removeDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 250; easing.type: Easing.OutQuad }
        }

        ScrollBar.vertical: ScrollBar { }
    }
}


