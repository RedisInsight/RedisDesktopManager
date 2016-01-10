import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

RowLayout {
   property alias model: repeater.model

   Repeater {
        id: repeater

        ToolButton {
            iconSource: modelData['icon']

            onClicked: {
                if (!connectionsManager)
                    return

                connectionsManager.sendEvent(styleData.index, modelData['event'])
            }
        }
    }
}
