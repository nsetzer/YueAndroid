import QtQuick 2.7
import QtQuick.Controls 2.2
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
            if (!progressBar.pressed)
                progressBar.value = mediaBar.fprogress;
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

    Slider {
        Timer {
            id: progressBarTimer
            interval: 100;
            repeat: false
            onTriggered: {
                if (!progressBar.pressed)
                    MediaPlayer.setProgress( progressBar.value );
            }
        }

        id: progressBar
        anchors.top : parent.top
        anchors.left: mediaBar.left
        anchors.right: mediaBar.right
        height: gDevice.textHeight
        from: 0.0
        to: 1.0
        stepSize: 0.01;
        live: false
        onMoved: {
            progressBarTimer.start();
        }
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
