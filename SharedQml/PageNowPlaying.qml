import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "." as SharedQml

PageBase {
    id: pageSettings

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

    Text {
        id: textTitle;
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        height: gDevice.textHeight * 2.5
        text: "title"
    }
    Text {
        id: textArtist;
        anchors.top: textTitle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        height: gDevice.textHeight * 2.5
        text: "artist"
    }
    Text {
        id: textAlbum;
        anchors.top: textArtist.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        height: gDevice.textHeight * 2.5
        text: "album"
    }

    Image {
        id: albumArt
        source: "qrc:/shared/images/00_song_art.svg"
        asynchronous: true
        anchors.top: textAlbum.bottom
        anchors.topMargin: gDevice.textHeight
        width: parent.width * .80
        height: width;
        anchors.horizontalCenter: parent.horizontalCenter
        sourceSize.width: width

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log(mediaBar.height)
            }
        }

    }

    MediaBarBig {
        id: mediaBar
        anchors.top:  albumArt.bottom
        height: gDevice.textHeight * 4
        width: parent.width

        color: "red"

    }
}
