import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.yue.common 1.0

import "qrc:/shared" as SharedQml


Image {
    id: root

    signal clicked();
    signal repeat();

    Timer {
        id: timerRepeat;
        interval: 100
        repeat: true
        onTriggered: {
            root.repeat();
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            root.clicked();
        }

        onPressAndHold: {
            timerRepeat.start();
        }

        onReleased: {
            timerRepeat.stop();
        }
    }
}
