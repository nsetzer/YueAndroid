import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import com.yue.common 1.0

import "qrc:/shared" as SharedQml

PageBase {
    id: explorer
    title: Explorer
    focus: true

    function openPage() {
        console.log("opening page: " + title);
    }

    DirectoryListModel { id: dirModel}
    Device {id: device}

    SimpleHeader {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left;
        anchors.right: parent.right;
        text: dirModel.currentDirectoryName
        onBackClicked: dirModel.openParentDirectory();
    }

    ListView {
        id: myList
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left;
        anchors.right: parent.right;
        clip: true

        model: dirModel
        delegate: SharedQml.ListItemDelegate {
            text: display
            rowIndex: index
            iconPath: icon
            onActivated: isdir ? dirModel.changeDirectory( index ) : dirModel.openFile( index );
            checked: dirModel.isSelected(index);
            onToggleCheckState: dirModel.toggleSelection(index);
            checkable: true
        }
        ScrollBar.vertical: ScrollBar { }
    } //listview

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: dirModel.count==0
        text: "Empty Directory"
        font.pointSize: device.font.pointSize + 3
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back) {
            event.accepted = true;
            dirModel.openParentDirectory()
        }
    }

}
