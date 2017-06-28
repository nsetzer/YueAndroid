import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.yue.common 1.0

import "qrc:/shared" as SharedQml

Item {

    property real indexWidth: 0
    property int  currentIndex: -1

    signal moreClicked(int index);

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
        width: indexWidth
        horizontalAlignment: Text.AlignRight
        font.pointSize: gDevice.font.pointSize - 1
    }
    AlbumArtImage {
        id: delegateIcon
        height: parent.height - gDevice.textHeight/4
        width: height
        seedString: album;
        anchors.left: txtIndex.right
        anchors.leftMargin: gDevice.textHeight/4
        anchors.verticalCenter: parent.verticalCenter
    }
    /*Image {
        id: delegateIcon
        height: parent.height
        width: height
        source: "qrc:/shared/images/00_song_art.svg"
        asynchronous: true
        // set the maximum height of the laoded image.
        // width will automaticall set to preserve aspect ratio.
        // this saves memory for large images.
        sourceSize.height: parent.height
        anchors.left: txtIndex.right
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
            anchors.fill: parent;
            //onClicked: {
            //    /console.log("remove: " + index + " " + plmodel.removeItem)
            //    //plmodel.removeItem(index)
            //}
        }

    }
    */
    Text {
        id: txtTitle
        anchors.top: parent.top
        anchors.left: delegateIcon.right
        anchors.leftMargin: gDevice.textHeight/4
        anchors.right: contextMenuIcon.left
        anchors.bottom: parent.verticalCenter
        text: title
        elide: Text.ElideRight
        verticalAlignment: Text.AlignBottom
        font.pointSize: gDevice.font.pointSize + 1
        color: (index == currentIndex) ? "red": "black"
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.left: delegateIcon.right
        anchors.leftMargin: gDevice.textHeight/4
        anchors.top: parent.verticalCenter
        id: txtArtist
        text: artist
        color: (index == currentIndex) ? "red": "black"
        verticalAlignment: Text.AlignTop
        font.pointSize: gDevice.font.pointSize - 1
    }

    Text {
        anchors.top: parent.verticalCenter
        anchors.right: contextMenuIcon.left
        anchors.rightMargin: 20
        text: formatDuration(duration)
        color: (index == currentIndex) ? "red": "black"
        font.pointSize: gDevice.font.pointSize - 1
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
                //MediaPlayer.playIndex( index );
                moreClicked(index)
            }
        }
    }

/*
    Rectangle {
        anchors.bottom: parent.bottom;
        height: 2
        width: parent.width
        color: "#33777777"
    }
    */

} // end item
