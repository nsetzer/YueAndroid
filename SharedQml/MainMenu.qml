import QtQuick 2.7
import QtQuick.Controls 1.2


Item {
    id: menu

    signal menuItemClicked( string item, string page )
    property alias currentItem: listViewMenu.currentIndex

    ListModel {
        id: modelMenu
        ListElement {
            item: "Now Playing"
            icon: "qrc:/shared/images/01_now_playing_a.svg"
            page: "qrc:/shared/PageSettings.qml"
        }
        ListElement {
            item: "Current Playlist"
            icon: "qrc:/shared/images/02_playlist_a.svg"
            page: "qrc:/shared/PagePlaylist.qml"
        }
        ListElement {
            item: "Library"
            icon: "qrc:/shared/images/03_music_library_a.svg"
            page: "qrc:/shared/PageLibrary.qml"
        }
        ListElement {
            item: "Playlists"
            icon: "qrc:/shared/images/05_ordered_list_a.svg"
            page: "qrc:/shared/SimpleListDemo.qml"
        }
        ListElement {
            item: "File System"
            icon: "qrc:/shared/images/00_folder.svg"
            page: "qrc:/shared/PageExplorer.qml"
        }
        ListElement {
            item: "Settings"
            icon: "qrc:/shared/images/06_settings_a.svg"
            page: "qrc:/shared/PageSettings.qml"
        }
    }

    Rectangle {
        id: logoWtapper
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        width: parent.width
        height: width*0.5
        color: palette.primary //"#3078fe" //this color is equal to the background of imgLogo
        clip: true
        Image {
            id: imgLogo
            source: "images/background.jpg"
            height: parent.height
            width: parent.width
            antialiasing: true
            smooth: true
            anchors.top: parent.top
            anchors.left: parent.left
            opacity: 0.5
        }
    }
    Image {
        id: imgShadow
        anchors.top: logoWtapper.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 10*app.dp
        z: 4
        source: "images/shadow_title.png"
    }
    ListView {
        id: listViewMenu
        anchors.top: logoWtapper.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        model: modelMenu
        delegate: componentDelegate
    }

    Component {
        id: componentDelegate

        Rectangle {
            id: wrapperItem
            height: 60*app.dp
            width: parent.width
            color: wrapperItem.ListView.isCurrentItem || ma.pressed ? palette.currentHighlightItem : "transparent"
            Image {
                id: imgItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 0.5*imgItem.width
                height: parent.height*0.75
                width: height
                sourceSize.width: height
                sourceSize.height: height
                source: icon
                visible: icon != ""
                //smooth: true
                //antialiasing: true
            }

            Label {
                id: textItem
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: imgItem.right
                anchors.leftMargin: 0.7*imgItem.width
                text: item
                font.pixelSize: parent.height*0.3
                color: wrapperItem.ListView.isCurrentItem ? palette.darkPrimary : palette.primaryText
            }


            MouseArea {
                id: ma
                anchors.fill: parent
                enabled: app.menuIsShown
                onClicked: {
                    menu.menuItemClicked( item, page )
                    listViewMenu.currentIndex = index
                }
            }
        }

    }
}

