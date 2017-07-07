import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQml.Models 2.2

import com.yue.common 1.0
import "constants.js" as Constants
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

PageBase {
    id: root

    title: qsTr("Library")

    width: 300; height: 400

    property bool createShuffledPlaylist: true

    LibraryTreeListModel {
        id: treeModel
        defaultQuery: app.defaultLibraryQuery
    }

    LineInput {
        id: searchInput
        text: app.defaultLibraryQuery
        hint: "Search Library"
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        height: gDevice.textHeight * 2.25
        onAccepted: {
            treeModel.search( searchInput.text );
        }
    }

    ListView {
        id: view
        anchors.top: searchInput.bottom
        anchors.bottom: buttonBar.top
        anchors.left: parent.left;
        anchors.right: parent.right;
        visible: treeModel.lastError == ""
        model: treeModel
        cacheBuffer: 100
        clip: true

        // default is 2500 on windows, with a device dp of 0.6
        // on android, dp is 2.625 for my nexus 5x
        // assume a linear scale, and a fudge factor.
        // this value approximates a good setting on all devices.
        maximumFlickVelocity: 4000*gDevice.dp

        delegate:  Rectangle {
            id: delegateItem
            height: targetHeight
            width: view.width

            readonly property int targetHeight: gDevice.textHeight * 2.25//delegateText.implicitHeight * 3

            ExpanderButton {
                id: delegateButton
                //text: "clickme"
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                // need some paddnig for depth==0 to
                // get the button out of the gutter
                anchors.leftMargin: (height/3) + (depth * (height/2))

                primaryColor: Constants.black
                secondaryColor: Constants.darkPrimary

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
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                anchors.right: (depth==2)?contextMenuIcon.left:parent.right
                anchors.left: delegateButton.right
                text: display
                font.bold: depth < 2
                MouseArea {
                    anchors.fill: parent
                    onClicked: treeModel.toggleCheckState( index )
                }

            }

            IconButton {
                id: contextMenuIcon
                height: parent.height*.66
                width: height
                visible: depth==2
                anchors.right : parent.right
                anchors.rightMargin: parent.height*.33
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/shared/images/00_menu_a.svg"
                color: "transparent"

                onClicked: menu.open()
            }

            /*
            Image {
                id: contextMenuIcon
                height: parent.height*.66
                visible: depth==2
                width: parent.height
                anchors.right : parent.right
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/shared/images/00_menu_a.svg"
                asynchronous: true
                sourceSize.height: parent.height*.66
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        menu.open()
                    }
                }
            }
            */

            Rectangle {
                /*anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.buttonBar
                */
                anchors.fill: parent
                visible: checkState == Qt.Checked
                color: (depth==2)?Constants.selectedLight:((depth==1)?Constants.selected:Constants.selectedDark)

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
                visible: display!="" // check for dummy row
                width: parent.width
                color: "#33777777"
            }

            Menu {
                id: menu
                y: delegateItem.height
                x: delegateItem.width - menu.width
                width: view.width / 2


                MenuItem {
                    text: "Play Song"
                    visible: index !== MediaPlayer.currentIndex
                    height: visible? gDevice.textHeight*2:0
                    onTriggered: {
                        MediaPlayer.playSong( uid )
                    }
                }
                MenuItem {
                    text: "Play Song Next"
                    visible: index !== MediaPlayer.currentIndex
                    height: visible? gDevice.textHeight*2:0
                    onTriggered: {
                        MediaPlayer.playNext( uid )
                    }
                }
            } // end menu
        }

        footer: Component {
            Rectangle {
                width: parent.width
                height: view.height/2
                color: "transparent"
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

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: treeModel.lastError != ""
        text: treeModel.lastError
        font.pointSize: gDevice.font.pointSize - 1
        width: 0.9 * parent.width;
        wrapMode: Text.WordWrap
        color: "orangered"
    }

    Item {

        id: buttonBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: gDevice.textHeight*2


        Button {
            id: btnCheckAll
            anchors.left: parent.left
            anchors.right: btnPLaylistOrder.left
            height: gDevice.textHeight*2
            text: treeModel.anySelected ? "Unselect All" : "Select All";

            onClicked: treeModel.checkAll( !treeModel.anySelected )

        }
        IconButton {
            id: btnPLaylistOrder
            height: gDevice.textHeight*2
            width: height
            anchors.horizontalCenter: parent.horizontalCenter
            source: createShuffledPlaylist ? "qrc:/shared/images/00_playlist_shuffle_a.png" : "qrc:/shared/images/00_playlist_ordered_a.png"

            onClicked: {
                createShuffledPlaylist = !createShuffledPlaylist;
            }

        } //image
        Button {
            id: btnCreatePlaylist
            anchors.left: btnPLaylistOrder.right
            anchors.right: parent.right
            height: gDevice.textHeight*2
            text: "Create Playlist"

            onClicked: {
                if (treeModel.createPlaylist(createShuffledPlaylist)) {
                    app.openCurrentPlaylist();
                }
            }
        }


    }
}


