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
        //x: 0 //parent.verticalCenter - height/2
        //y: 0 // parent.width - width

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

    Component {
        id: dragDelegate


        MouseArea {
            id: dragArea

            property bool held: false
            property bool swipe_: false

            property int swipeTreshold: 32
            property bool ready: false
            property point _origin
            property point _position
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

            //anchors.left: parent.left
            //anchors.right: parent.right
            width: view.width
            height: content.height

            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis

            /*
            onPressAndHold: {
                held = true
            }
            */
            Timer {
                id: timerPressAndHold
                repeat: false
                interval: pressAndHoldDuration;
                onTriggered: {
                    held = true
                }
            }

            onPressed: {
                drag.axis = Drag.XAndYAxis
                //drag.axis = Drag.YAxis
                _origin = Qt.point(mouse.x, mouse.y)
                disableAutoScroll()

                timerPressAndHold.start()
            }

            onPositionChanged: {
                _position = Qt.point(mouse.x, mouse.y)
                timerPressAndHold.stop()
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

                switch (drag.axis) {
                    case Drag.XAndYAxis:
                        if (Math.abs(mouse.x - _origin.x) > swipeTreshold) {
                            drag.axis = Drag.XAxis
                            swipe_ = true
                            held=true
                            timerPressAndHold.stop()
                        } else if (Math.abs(mouse.y - _origin.y) > swipeTreshold) {
                            drag.axis = Drag.YAxis
                        }
                        break

                    case Drag.XAxis: {
                        xoffset = mouse.x - _origin.x;
                        move(xoffset, 0);
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
                timerPressAndHold.stop()
                held = false
                swipe_= false;
                disableAutoScroll()

                var distance;
                switch (drag.axis) {
                case Drag.XAndYAxis: canceled(mouse) ; break
                case Drag.XAxis: {
                    distance = mouse.x - _origin.x
                    console.log("swipe distance LR: " + distance + " width: " + parent.width)
                    if (Math.abs(distance)/parent.width < 1/3)
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

                //border.width: 1
                //border.color: "lightsteelblue"

                color: (dragArea.held ? ((Math.abs(xoffset)/parent.width > 1/3)?"orangered":"lightsteelblue") : "white")
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
                    width: parent.width

                    onMoreClicked: {
                        //MediaPlayer.playIndex(index)
                        //menu.y= delegate.height
                        //menu.x= delegate.width - menu.width
                        menu.open()
                    }

                    Menu {
                        id: menu
                        y: delegate.height
                        x: delegate.width - menu.width
                        width: view.width / 2


                        MenuItem {
                            text: "Play Song"
                            visible: index !== MediaPlayer.currentIndex
                            height: visible? gDevice.textHeight*2:0
                            onTriggered: {
                                MediaPlayer.playIndex(index)
                            }
                        }
                        MenuItem {
                            text: "Play Song Next"
                            visible: index !== MediaPlayer.currentIndex
                            height: visible? gDevice.textHeight*2:0
                            onTriggered: {
                                if (index !== MediaPlayer.currentIndex) {
                                    listModel.move(index,MediaPlayer.currentIndex+1)
                                }
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

                }

                Rectangle {
                    anchors.bottom: parent.bottom;
                    height: 2
                    width: parent.width
                    color: "#33777777"
                }
            }

            DropArea {

                anchors { fill: parent; }

                onEntered: {
                   listModel.move(
                        drag.source.DelegateModel.itemsIndex,
                        dragArea.DelegateModel.itemsIndex);
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

