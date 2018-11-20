import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.yue.common 1.0

import "qrc:/shared" as SharedQml


PageBase {
    title: "Dynamic Playlists"

    function openPage() {
        console.log("opening page: " + title);
    }

    function titleClicked() {
        console.log("Title Clicked on page" + title)
    }
    ListModel {
            id: listModel

            ListElement {  name: "Best: English"; query: "lang=english" }
            ListElement {  name: "Best: Visual"; query: "(\"visual kei\" || genre=j-metal ||(genre =j-rock && genre = \"nu metal\")) && rating > 0" }
            ListElement {  name: "Best: Grunge"; query: "genre=grunge rating>3 ban=0" }
            ListElement {  name: "Best: Stoner"; query: "stoner" }
            ListElement {  name: "Genre: Grunge"; query: "genre=grunge rating>0 ban=0" }
            ListElement {  name: "Genre: Visual"; query: "(\"visual kei\" || genre=j-metal ||(genre =j-rock && genre = \"nu metal\"))" }
            ListElement {  name: "John Garcia"; query: "John Garcia" }
            ListElement {  name: "Gothic Emily"; query: "album = \"gothic emily\"" }
            ListElement {  name: "[tai]"; query: "[tai]" }


        }

    Component{
        id: dynComponent;
        Rectangle {
            height: gDevice.textHeight * 3
            width: parent.width
            Text {
                anchors.top: parent.top
                anchors.bottom: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
                verticalAlignment: Text.AlignVCenter
                text: name
                font.bold: true
            }
            Text {
                anchors.top: parent.verticalCenter
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                verticalAlignment: Text.AlignVCenter
                text: "<b>Search:</b> " + query
            }

            Image {
                id: contextMenuIcon
                height: parent.height*.66
                width: height
                anchors.right : parent.right
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/shared/images/00_menu_a.svg"
                asynchronous: true
                sourceSize.height: height

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        console.log("context: " + index )

                        openLibrarySearch(query);

                        //MediaPlayer.playIndex( index );
                        moreClicked(index)
                    }
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom;
                height: 2
                width: parent.width
                color: "#33777777"
            }

        }
    }

    ListView {
        id: view
        anchors.fill: parent
        clip: true
        maximumFlickVelocity: 4000*gDevice.dp

        model: listModel
        delegate: dynComponent
        ScrollBar.vertical: ScrollBar { }
        spacing: gDevice.textHeight/3

        remove: Transition {
            NumberAnimation { property: "opacity"; to:0; duration:250; }
        }
        removeDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration:250; }
        }
    } //listview

}
