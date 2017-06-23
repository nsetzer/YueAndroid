import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
    id: page

    clip: true
    property string title: "Title"

    function openPage() {
        console.log("opening page: " + title);
    }

    function titleClicked() {
        console.log("Title Clicked on page" + title)
    }

    Rectangle {
        id: curtain
        color: "black"
        opacity: app._progressOpening*0.5
        anchors.fill: parent
        z: 100
    }
}


