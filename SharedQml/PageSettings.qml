import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "." as SharedQml

PageBase {
    id: pageSettings

    title: qsTr("Settings")

    function openPage() {
        console.log("opening page: " + title);
    }

    Label {
        anchors.centerIn: parent
        text: qsTr("Settings")
    }
}
