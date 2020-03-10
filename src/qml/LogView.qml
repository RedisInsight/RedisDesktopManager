import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13

ScrollView {
    id: root

    property alias eventsModel: modelConnections.target

    ListView {
        id: logListView
        anchors.fill: parent
        anchors.topMargin: 10
        anchors.bottomMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        cacheBuffer: 0

        model: ListModel {
            id: logModel
        }

        Connections {
            id: modelConnections
            onLog: {
                if (logModel.count > 1500) {
                    logModel.remove(0, logModel.count - 1000)
                }

                logModel.append({"msg": msg})
                logListView.positionViewAtEnd()
            }
        }

        delegate: Text {
            color: "#6D6D6E"
            text: msg
        }
    }
}
