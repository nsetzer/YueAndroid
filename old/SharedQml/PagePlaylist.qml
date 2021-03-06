import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.yue.common 1.0

import "qrc:/shared" as SharedQml

PageBase {
    id: explorer

    title: "Current Playlist"

    function openPage() {
        console.log("opening page: " + title);
        //plmodel.playlistName = ":current:"
    }

    function titleClicked() {
        console.log("Title Clicked on page" + title)
    }

    function formatDuration(s) {

        var h = Math.floor(s / 3600);
        var m = Math.floor( (s-(h*60)) / 60);
        var s = s - (h*60) - (m*60);

        if (h > 0) {
            h = h + ":"
        } else {
            h = ""
        }

        if (m < 10) {m = "0"+m;}
        if (s < 10) {s = "0"+s;}
        return h+m+':'+s;
    }

    FontMetrics {
        id: fontMetrics
        //font:
    }

    PlaylistModel {
        id: plmodel
        playlistName: ":current:"

    }

    TextMetrics {
        id: textMetricsIndex
        text: "000."
    }

    Component {
            id: delegate
            Item {
                id: delegateItem
                width: parent.width
                height: gDevice.textHeight * 3

                anchors.leftMargin: 20
                anchors.rightMargin: 20

                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    id: txtIndex
                    text: (index+1) + "."
                    width: textMetricsIndex.width
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: fontMetrics.font.pointSize - 1
                }

                Image {
                    id: delegateIcon
                    height: parent.height
                    width: height
                    source: "qrc:/shared/images/06_settings_b.svg"
                    asynchronous: true
                    // set the maximum height of the laoded image.
                    // width will automaticall set to preserve aspect ratio.
                    // this saves memory for large images.
                    sourceSize.height: parent.height-4
                    anchors.left: txtIndex.right
                    anchors.verticalCenter: parent.verticalCenter

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            console.log("remove: " + index + " " + plmodel.removeItem)
                            plmodel.removeItem(index)
                        }
                    }

                }

                Text {
                    id: txtTitle
                    anchors.top: parent.top
                    anchors.left: delegateIcon.right
                    anchors.right: contextMenuIcon.left
                    anchors.bottom: parent.verticalCenter
                    text: title
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignBottom
                    font.pointSize: fontMetrics.font.pointSize + 1
                    color: (index == MediaPlayer.currentIndex) ? "red": "black"
                }

                Text {
                    anchors.bottom: parent.bottom
                    anchors.left: delegateIcon.right
                    anchors.top: parent.verticalCenter
                    id: txtArtist
                    text: artist
                    color: (index == MediaPlayer.currentIndex) ? "red": "black"
                    verticalAlignment: Text.AlignTop
                    font.pointSize: fontMetrics.font.pointSize - 1
                }

                Text {
                    anchors.top: parent.verticalCenter
                    anchors.right: contextMenuIcon.left
                    anchors.rightMargin: 20
                    text: formatDuration(duration)
                    color: (index == MediaPlayer.currentIndex) ? "red": "black"
                    font.pointSize: fontMetrics.font.pointSize - 1
                }

                Image {
                    id: contextMenuIcon
                    height: parent.height
                    width: parent.height
                    anchors.right : parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/shared/images/04_dynamic_a.svg"
                    asynchronous: true
                    sourceSize.height: parent.height
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            console.log("context: " + index )
                            MediaPlayer.playIndex( index );
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
        id: myList
        anchors.top: parent.top
        anchors.bottom: mediaBar.top
        anchors.left: parent.left;
        anchors.right: parent.right;
        clip: true
        maximumFlickVelocity: 4000*gDevice.dp

        model: plmodel
        delegate: delegate
        ScrollBar.vertical: ScrollBar { }

        remove: Transition {
            NumberAnimation { property: "opacity"; to:0; duration:250; }
        }
        removeDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration:250; }
        }
    } //listview


    MediaBar {
        id: mediaBar
        anchors.left : parent.left
        anchors.right : parent.right
        anchors.bottom : parent.bottom
        height: gDevice.textHeight * 2 + margin
        margin: gDevice.textHeight/4

        onPrevClicked: {
            console.log("prev")
            MediaPlayer.setProgress(0.99)
        }

        onNextClicked: {
            MediaPlayer.next();
            console.log("next")
        }

        onPlayPauseClicked: {
            console.log("playpause")
            MediaPlayer.playpause();
        }
    }

}
