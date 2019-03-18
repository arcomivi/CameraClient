import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12

Window {
    visible: true
    width: Qt.platform.os==="windows" ? 480 : Screen.width
    height: Qt.platform.os==="windows" ? 800 : Screen.height
    title: qsTr("Hello World")
    color: "lightgrey"
    Image {
        id: liveImage
        property bool counter: false

        asynchronous: true
        source: "image://live/image"
        anchors.top: parent.top
        anchors.left: parent.left
        height: parent.height/2
        width: parent.width
        fillMode: Image.PreserveAspectFit
        cache: false


        function reload() {
            counter = !counter
            source = "image://live/image?id=" + counter
        }
    }
    VideoOutput {
        id: streamedCamera
        objectName: "display"
        height: parent.height/2
        width: parent.width
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        source: $provider
    }
    Rectangle {
        width: parent.width;
        height: 50
        anchors.bottom: parent.bottom;
        MouseArea {
            anchors.fill: parent;
            onClicked: liveImage.reload();
        }
    }
}
