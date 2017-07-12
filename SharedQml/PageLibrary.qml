import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQml.Models 2.2

import com.yue.common 1.0

PageBase {

    title: "Library"

    function openPage() {
        console.log("opening page: " + title);
    }

    function titleClicked() {
        console.log("Title Clicked on page" + title)
    }

    function showMoreButton() {
        return true
    }

    function moreClicked() {
        console.log("More clicked on page " + title)
        MediaPlayer.tts()
    }



    PageIndicator {
        id: indicator

        count: view.count
        currentIndex: view.currentIndex

        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter


    }

    Rectangle {
        id: shadow
        anchors.top: indicator.bottom;
        height: 2
        width: parent.width
        color: "#FF333333"
    }


    SwipeView {
        id: view

        currentIndex: 0
        anchors.top: shadow.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        PageLibraryTree {
            id: firstPage
        }
        Item {
            id: secondPage
        }
        Item {
            id: thirdPage
        }


    }
}
