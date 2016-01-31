import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "./../../"

RowLayout {
   property alias model: repeater.model
   property var callbacks

   Repeater {
        id: repeater

        ToolButton {
            iconSource: modelData['icon']

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: {
                if (!connectionsManager)
                    return

                if (modelData['callback'] != undefined) {
                    return callbacks[modelData['callback']]()
                }

                connectionsManager.sendEvent(styleData.index, modelData['event'])
            }

            tooltip: modelData['help'] != undefined ? modelData['help'] : ""
        }
    }
}
