import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

RowLayout {
   property alias model: repeater.model
   property var callbacks

   Repeater {
        id: repeater

        ToolButton {
            iconSource: modelData['icon']

            onClicked: {
                if (!connectionsManager)
                    return

                if (modelData['callback'] != undefined) {
                    return callbacks[modelData['callback']]()
                }

                connectionsManager.sendEvent(styleData.index, modelData['event'])
            }
        }
    }
}
