import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2

import com.yue.common 1.0

/*

  TODO:

  https://stackoverflow.com/questions/28729729/qml-is-it-possible-to-change-the-long-duration-of-a-mouse-area
  use a custom timer for press and hold
  and cancel the timer if the usre drags out of a small radius

  if the user drags left or right, drag the row left or right
  emit a swipe if the release happens after dragging more than
  50% of the width

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

PageBase {
    id: root

    title: "Current Playlist"

    function openPage() {
        console.log("opening page: " + title);
    }

    function titleClicked() {
        // jump the list view to center on the current item
        view.positionViewAtIndex(MediaPlayer.currentIndex,ListView.Center);
    }

    width: 300; height: 400

    SongListModel {
        id: listModel
        playlistName: ":current:"
    }

    TextMetrics {
        id: textMetricsIndex
        text: "000."
    }

    Component {
        id: dragDelegate


        MouseArea {
            id: dragArea

            property bool held: false

            property int swipeTreshold: 32
            property bool ready: false
            property point _origin
            property int xoffset: 0
            property int yoffset: 0

            readonly property string dirUp:     "up"
            readonly property string dirDown:   "down"
            readonly property string dirLeft:   "left"
            readonly property string dirRight: "right"

            signal move(int x, int y)
            signal swipeRight(int distance)
            signal swipeLeft(int distance)
            signal swipeUp(int distance)
            signal swipeDown(int distance)

            anchors.left: parent.left
            anchors.right: parent.right
            height: content.height

            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis

            onPressAndHold: {
                held = true
            }

            onPressed: {
                drag.axis = Drag.XAndYAxis
                _origin = Qt.point(mouse.x, mouse.y)
            }

            onPositionChanged: {
                switch (drag.axis) {
                    case Drag.XAndYAxis:
                        if (Math.abs(mouse.x - _origin.x) > swipeTreshold)
                            drag.axis = Drag.XAxis
                        else if (Math.abs(mouse.y - _origin.y) > swipeTreshold)
                            drag.axis = Drag.YAxis
                        break

                    case Drag.XAxis: {
                        xoffset = mouse.x - _origin.x;
                        move(xoffset, 0);
                        //console.log(xoffset)
                        //held = true
                        break
                    }
                    case Drag.YAxis: {
                        yoffset = mouse.y - _origin.y;
                        move(0, yoffset);
                        break
                    }
                }
            }

            onReleased: {
                held = false
                var distance;
                switch (drag.axis) {
                case Drag.XAndYAxis: canceled(mouse) ; break
                case Drag.XAxis: {
                    distance = mouse.x - _origin.x
                    console.log("swipe distance LR: " + distance + " width: " + parent.width)
                    if (Math.abs(distance)/parent.width < 1/2)
                        return
                    if ( distance < 0) {
                        swipeLeft(Math.abs(distance))
                        console.log("left")
                    } else {
                        swipeRight(Math.abs(distance))
                        console.log("right")
                    }
                    break
                }
                case Drag.YAxis: {
                    distance = mouse.y - _origin.y

                    //swipedir(dir, Math.abs(mouse.y-_origin.y)) ; break
                    console.log("swipe distance UD: " + distance + " width: " + parent.height)
                    if ( distance < 0) {
                        swipeUp(Math.abs(distance))
                    } else {
                        swipeDown(Math.abs(distance))
                    }
                    break
                }
                }
            }

            Rectangle {
                id: content
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }

                width: dragArea.width;
                height: gDevice.textHeight * 3

                border.width: 1
                border.color: "lightsteelblue"

                color: (dragArea.held ? ((Math.abs(xoffset)/parent.width > 1/2)?"gold":"lightsteelblue") : "white")
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

                PlaylistItemDelegate {
                    id: delegate
                    anchors.fill: parent
                    indexWidth: textMetricsIndex.width
                    currentIndex: listModel.currentIndex
                }

            }

            DropArea {
                // TODO: fixed pixel margin
                anchors { fill: parent; margins: 10 }

                onEntered: {
                   listModel.move(
                        drag.source.DelegateModel.itemsIndex,
                        dragArea.DelegateModel.itemsIndex);
                }
            }

            onSwipeRight: {
                listModel.remove( dragArea.DelegateModel.itemsIndex );
            }
            onSwipeLeft: {
                listModel.remove( dragArea.DelegateModel.itemsIndex );
            }


        }

    }

    ListView {
        id: view

        anchors.top: parent.top
        anchors.bottom: mediaBar.top
        anchors.left: parent.left;
        anchors.right: parent.right;

        //model: visualModel
        model: listModel
        delegate: dragDelegate

        maximumFlickVelocity: 4000*gDevice.dp


        spacing: 4
        cacheBuffer: 100

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 250; easing.type: Easing.OutQuad }
        }

        remove: Transition {
            NumberAnimation {
                properties: "height";
                to: 0;
                duration: 250;
                easing.type: Easing.OutQuad
            }
        }

        ScrollBar.vertical: ScrollBar { }
        /*
        DropArea {
            anchors.top: view.top
            anchors.left: view.left
            anchors.right: view.right
            height: gDevice.textHeight * 2

            Rectangle {
                anchors.fill: parent
                color: "#22FF0000"
            }

            onEntered: {
                console.log("updrop")
            }
        }
        */

    }

    MediaBar {
        id: mediaBar
        anchors.left : parent.left
        anchors.right : parent.right
        anchors.bottom : parent.bottom
        height: gDevice.textHeight * 2 + margin
        margin: gDevice.textHeight/4

        onPrevClicked: {
            MediaPlayer.prev();
        }

        onNextClicked: {
            MediaPlayer.next();
        }

        onPlayPauseClicked: {
            MediaPlayer.playpause();
        }
    }
}

