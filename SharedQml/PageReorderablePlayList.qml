import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2

import com.yue.common 1.0
import "constants.js" as Constants
/*

  TODO:


  on tap: display a delete icon
  remove swipe to delete
  keep drag and drop to reorder

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

    function min(x,y) {
        if (x<y)
            return x
        return y
    }

    function max(x,y) {
        if (x>y)
            return x
        return y
    }

    function globalPositionImpl(node,stopAt) {
        var returnPos = {};
        returnPos.x = 0;
        returnPos.y = 0;
        if(node !== undefined && node !== null && node !== stopAt) {
            var parentValue = globalPosition(node.parent);
            returnPos.x = parentValue.x + node.x;
            returnPos.y = parentValue.y + node.y;
        }
        return returnPos;
    }

    function globalPosition(node) {
        return globalPositionImpl(node,null)
    }

    property int pressAndHoldDuration: 200 // ms
    property int autoScrollMargin: (gDevice.textHeight * 4.5)
    property int autoScrollStep: gDevice.textHeight * .5
    property int autoScrollDirection: 0
    property int autoScrollIncrements: 0
    property var autoScrollTarget: null


    Timer {
        id: timerAutoScroll
        interval: 33//max(25,150/(autoScrollIncrements+5)/5)
        repeat: true
        onTriggered: {
            autoScrollIncrements+=1
            var rate = autoScrollDirection * autoScrollStep
            view.contentY =  min(view.contentHeight,max(0,view.contentY+rate))


            if (autoScrollTarget!==null) {
                autoScrollTarget.onAutoScrollCheckMouse()
            }
        }
    }


    function enableAutoScrollUp(bFast) {
        if (autoScrollDirection>=0) {
            autoScrollIncrements=0
        }
        autoScrollDirection = bFast?-2.5:-1
        timerAutoScroll.start()
    }
    function enableAutoScrollDown(bFast) {
        if (autoScrollDirection<=0) {
            autoScrollIncrements=0
        }
        autoScrollDirection = bFast?2.5:1
        timerAutoScroll.start()
    }
    function disableAutoScroll() {
        autoScrollIncrements = 0;
        autoScrollDirection = 0;
        timerAutoScroll.stop()
        autoScrollTarget = null

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

    Popup {
        id: popup

        leftPadding: parent.width/3
        rightPadding: gDevice.textHeight
        //width: parent.width/3
        //height: 100
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        Rectangle {

            Column {
            Button{id: btn1; text:"textA"}
            Button{id: btn2; text:"textB"}
            Button{id: btn3; text:"textC"}
            }

            color: "#33FFFF00"
        }
    }

    /*
    Component {
        id: itemContent

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            width: parent.width;
            height: gDevice.textHeight * 3



            Rectangle {
                anchors.bottom: parent.bottom;
                height: 2
                width: parent.width
                color: "#33777777"
            }
        }
    }
    */


    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false

            property point _position

            anchors { left: parent.left; right: parent.right }
            height: content.height

            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis

            onPressAndHold: held = true
            onReleased: held = false

            onPositionChanged: {
                _position = Qt.point(mouse.x, mouse.y)
                if (held) {
                    var view_y = globalPosition(view).y;
                    var drag_y = globalPosition(dragArea).y + mouse.y;
                    var region_up = view_y + autoScrollMargin
                    var region_dn = view_y + view.height - autoScrollMargin
                    if (drag_y < region_up) {
                        enableAutoScrollUp(drag_y < (view_y + (autoScrollMargin/2)))
                        autoScrollTarget = dragArea
                        //console.log("enable auto scroll up   : " + drag_y + " < " + region_up)
                    } else if (drag_y > region_dn) {
                        enableAutoScrollDown(drag_y > (view_y + view.height - (autoScrollMargin/2)))
                        autoScrollTarget = dragArea
                        //console.log("enable auto scroll down : " + drag_y + " > " + region_dn)
                    } else {
                        disableAutoScroll()
                        //console.log("disable auto scroll")
                    }
                }
            }

            Rectangle {
                id: content
//![0]
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                width: dragArea.width;
                height: gDevice.textHeight * 3

                border.width: 1
                border.color: "lightsteelblue"

                color: dragArea.held ? "lightsteelblue" : "white"
                Behavior on color { ColorAnimation { duration: 100 } }

                radius: 2
//![1]
                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
//![1]
                states: State {
                    when: dragArea.held

                    ParentChange { target: content; parent: root }
                    AnchorChanges {
                        target: content
                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
                    }
                }

                PlaylistItemDelegate {
                    id: playlistDelegate
                    anchors.fill: parent
                    indexWidth: textMetricsIndex.width
                    currentIndex: listModel.currentIndex
                    width: parent.width

                    onMoreClicked: {
                        //MediaPlayer.playIndex(index)
                        //menu.y= delegate.height
                        //menu.x= delegate.width - menu.width
                        menu.open()
                    }

                    Menu {
                        id: menu
                        y: playlistDelegate.height
                        x: playlistDelegate.width - menu.width
                        width: view.width / 2

                        MenuItem {
                            text: "Play Song"
                            visible: index !== listModel.currentIndex
                            height: visible? gDevice.textHeight*2:0
                            onTriggered: {
                                MediaPlayer.playIndex(index)
                            }
                        }
                        MenuItem {
                            text: "Play Song Next"
                            visible: index !== listModel.currentIndex && index !== (1+listModel.currentIndex)
                            height: visible? gDevice.textHeight*2:0
                            onTriggered: {
                                if (index !== listModel.currentIndex) {
                                    listModel.move(index,listModel.currentIndex+1)
                                }
                            }
                        }
                        MenuItem {
                            height: visible? gDevice.textHeight*2:0
                            visible: index !== listModel.currentIndex
                            text: "Remove Song"
                            onTriggered: {
                                listModel.remove(index)
                            }
                        }
                        MenuItem {
                            height: visible? gDevice.textHeight*2:0
                            text: "Search For Artist"
                            onTriggered: {
                                openLibrarySearchArtist(artist)
                            }
                        }
                        MenuItem {
                            height: visible? gDevice.textHeight*2:0
                            text: "Search For Album"
                            onTriggered: {
                                openLibrarySearchAlbum(artist, album)
                            }
                        }

                    } // end menu

                } // end delegate playlist view
//![2]
            }
//![3]
            DropArea {
                anchors { fill: parent; margins: 10 }

                onEntered: {
                    listModel.move(
                            drag.source.DelegateModel.itemsIndex,
                            dragArea.DelegateModel.itemsIndex)
                }
            }

            function onAutoScrollCheckMouse() {

                // when auto scroll is in effect mouse events and drop events
                // dont trigger. check the position of the mouse in the view
                // and mimic the droparea effect of moving the item

                if (held) {
                    var pos = globalPositionImpl(_position,view)
                    var index = view.indexAt(0,view.contentY+pos.y)
                    if (index>=0) {
                        listModel.move(dragArea.DelegateModel.itemsIndex, index)
                    }
                }

            }
//![3]
        }
    }
//![2]
//![4]
    DelegateModel {
        id: visualModel

        model: listModel
        delegate: dragDelegate
    }


    ListView {
        id: view

        anchors.top: parent.top
        anchors.bottom: mediaBar.top
        anchors.left: parent.left;
        anchors.right: parent.right;

        model: visualModel

        maximumFlickVelocity: 4000*gDevice.dp

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
    }

    MediaBar {
        id: mediaBar
        anchors.left : parent.left
        anchors.right : parent.right
        anchors.bottom : parent.bottom
        height: gDevice.textHeight * 2 + margin
        margin: gDevice.textHeight/4
        color: Constants.darkPrimary

        onPrevClicked: {
            MediaPlayer.prev();
        }

        onNextClicked: {
            MediaPlayer.next();
        }

        onSeekBackward: {
            MediaPlayer.setProgress(MediaPlayer.getProgress()-0.01);
        }

        onSeekForward: {
            MediaPlayer.setProgress(MediaPlayer.getProgress()+0.01);
        }

        onPlayPauseClicked: {
            MediaPlayer.playpause();
        }
    }
}

