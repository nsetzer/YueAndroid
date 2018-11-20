import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.yue.common 1.0

import "qrc:/shared" as SharedQml

Rectangle {
    id: mediaBar

    signal prevClicked();
    signal nextClicked();
    signal seekBackward();
    signal seekForward();
    signal playPauseClicked();

    property int margin: 2
    property real fprogress: 0
    property real resolution: 256

    property int playerState: 0
    property int playerStatus: 0

    function getPlayPauseIcon() {
        if (MediaPlayer.getStatus() == 1/*error*/) {
            return "qrc:/shared/images/00_song_error_a.svg"
        } else if (MediaPlayer.getState()==1/*playing*/) {
            return "qrc:/shared/images/00_song_pause.svg"
        } else {
            return "qrc:/shared/images/00_song_play.svg"
        }
    }

    Connections {
        target : MediaPlayer
        onProgressChanged: {
            mediaBar.fprogress = progress
        }
        /*onCurrentIndexChanged: {
            console.log(MediaPlayer.currentIndex)
        }*/
        onStatusChanged: {
            if (status == 1/*error*/) {
                imgSongPlayPause.source = "qrc:/shared/images/00_song_error_a.svg"
            }
        }

        onStateChanged: {
            imgSongPlayPause.source = getPlayPauseIcon();
        }
    }

    Rectangle {
        id: progressBarShadow
        anchors.left : parent.left
        anchors.top : parent.top

        height: margin
        width: parent.width
        color: "#888888"

    }

    Rectangle {
        id: progressBar
        anchors.left : parent.left
        anchors.top : parent.top

        height: margin
        width: parent.width * fprogress
        color: "blue"

    }

    Rectangle {
        anchors.left : parent.left
        anchors.right : parent.horizontalCenter
        anchors.top : progressBar.bottom
        anchors.bottom : parent.bottom
        color: "transparent"

        MediaSeekButton {
            id: imgSongPrev
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height
            width: height
            source: "qrc:/shared/images/00_song_prev.svg"
            asynchronous: true
            sourceSize.height: parent.height

            onClicked: prevClicked()
            onRepeat: seekBackward();

        }
    }

    Rectangle {
        anchors.left : parent.horizontalCenter
        anchors.right : parent.right
        anchors.top : progressBar.bottom
        anchors.bottom : parent.bottom
        color: "transparent"

        MediaSeekButton {
            id: imgSongNext
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height
            width: height
            source: "qrc:/shared/images/00_song_next.svg"
            asynchronous: true
            sourceSize.height: parent.height

            onClicked: nextClicked()
            onRepeat: seekForward();

        }
    }

    Image {
        id: imgSongPlayPause
        anchors.top : progressBar.bottom
        anchors.bottom : parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height
        width: height
        source: getPlayPauseIcon();
        asynchronous: true
        sourceSize.height: parent.height

        MouseArea {
            anchors.fill: parent
            onClicked: playPauseClicked()
        }
    }

}
