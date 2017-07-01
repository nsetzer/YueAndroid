import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "." as SharedQml
import com.yue.common 1.0

PageBase {
    id: root

    title: "Now Playlist"

    function openPage() {
        console.log("opening page: " + title);
    }

    function titleClicked() {
        console.log("Title Clicked on page" + title)
    }

    function min(x,y) {
        if (x<y)
            return x
        return y
    }

    /*
    SongListModel {
        id: listModel
        playlistName: ":current:"
    }*/

    Text {
        id: textTitle;
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        height: gDevice.textHeight * 2.5
        text: MediaPlayer.currentSong.title
    }
    Text {
        id: textArtist;
        anchors.top: textTitle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        height: gDevice.textHeight * 2.5
        text: MediaPlayer.currentSong.artist
    }
    Text {
        id: textAlbum;
        anchors.top: textArtist.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        height: gDevice.textHeight * 2.5
        text: MediaPlayer.currentSong.album
    }
/*
    Component {
        id: songDelegate

        Rectangle {
            color: "#22FF0000"
            Text {
                text:   "index"
            }

            AlbumArtImage {
                id: albumArt
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: 128; //root.width * .80
                height: width;
                seedString: "album"
                MouseArea {
                    anchors.fill: parent;
                    onClicked: albumArt.regenerate();
                }
            }
        }
    }*/

    /*
    PathView {
        id: view
        anchors.top: textAlbum.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: mediaBar.top

        pathItemCount: 3

        model: listModel
        delegate: songDelegate

        property real hw: view.width/2
        property real hh: view.height/2

        path: Path {



            startX: view.hw/2;
            startY: view.hh/4
            PathLine { x: view.hw; y: view.hh; }
            PathLine { x: 3*view.hw/2; y: view.hh/4; }
        }

        MouseArea{
           anchors.fill: parent
           onWheel: {
                if( wheel.angleDelta.y > 0 )
                    view.incrementCurrentIndex();
                else
                    view.decrementCurrentIndex();
           }
        }

    }*/

    Item {

        anchors.top: textAlbum.bottom
        anchors.topMargin: gDevice.textHeight
        anchors.bottom: mediaBar.top
        anchors.left: parent.left
        anchors.right: parent.right

        AlbumArtImage {
            id: albumArt
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * .80
            height: width;
            implicitWidth: width
            implicitHeight: height
            seed: MediaPlayer.currentSong.songid;
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(mediaBar.height)
                    albumArt.regenerate();
                }
            }

        }
    }



    MediaBarBig {
        id: mediaBar
        anchors.bottom:  parent.bottom
        height: gDevice.textHeight * 4
        width: parent.width

        color: "red"

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

