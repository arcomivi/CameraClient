import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    color: "lightgrey"
    Image {
        id: liveImage
        property bool counter: false

        asynchronous: true
        source: "image://live/image"
        anchors.top: parent.top
        anchors.left: parent.left
        height: 100
        width: 100
        fillMode: Image.PreserveAspectFit
        cache: false


        function reload() {
            counter = !counter
            source = "image://live/image?id=" + counter
        }
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
    VideoOutput {
        id: streamedCamera
        objectName: "display"
        height: 300
        width: 300
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        source: $provider
    }
}
