import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Rectangle {
      id: root
      height:  delegateText.height * 3;
      width: parent.width

      signal activated()
      signal toggleCheckState()

      property string text
      property color textColor: "black"
      property color backgroundColor: "transparent"
      property string iconPath: ""
      property int rowIndex // I cant name this "index"
                            // because that would alias the special
                            // value "index" -- the integer row value
      property bool checked:  false
      property bool checkable: false


      color: backgroundColor

      function min(a,b)
      {
          return (a>b) ? b : a;
      }

      CheckBox {
          id: delegateCheckBox
          anchors.left: parent.left
          anchors.verticalCenter: parent.verticalCenter
          onClicked: root.toggleCheckState()
          checked: root.checked
          visible: root.checkable
          enabled: root.checkable
      }

      Image {
          id: delegateIcon
          height: min(parent.height-4,sourceSize.height)
          width: height
          source: iconPath
          visible: iconPath != ""
          asynchronous: true
          // set the maximum height of the laoded image.
          // width will automaticall set to preserve aspect ratio.
          // this saves memory for large images.
          sourceSize.height: parent.height-4
          anchors.left: checkable ? delegateCheckBox.right : parent.left
          anchors.verticalCenter: parent.verticalCenter
      }
      Text {
          anchors.left: delegateIcon.visible ? delegateIcon.right : (delegateCheckBox.visible? delegateCheckBox.right : parent.left)
          anchors.right: parent.right
          anchors.verticalCenter: parent.verticalCenter
          anchors.leftMargin: root.width * 0.01
          id: delegateText
          text: root.text
          elide: Text.ElideMiddle
          color: root.textColor
      }

      MouseArea {
          anchors.left: checkable ? delegateCheckBox.right : parent.left
          anchors.right: root.right
          anchors.top: root.top
          anchors.bottom: root.bottom
          // put a small amount of space between mouse regions
          anchors.topMargin: 2
          anchors.bottomMargin: 2

          onClicked: root.activated( )

      }

      /*
      Rectangle {
          // visualize the mouse region
          anchors.left: checkable ? delegateCheckBox.right : parent.left
          anchors.right: root.right
          anchors.top: root.top
          anchors.bottom: root.bottom
          // put a small amount of space between mouse regions
          anchors.topMargin: 2
          anchors.bottomMargin: 2

          color: "#220000FF"
      }
      */

      // visual separator along bottom of item
      Rectangle {
          anchors.bottom: parent.bottom;
          height: 2
          width: parent.width
          color: "#33777777"
      }

}



