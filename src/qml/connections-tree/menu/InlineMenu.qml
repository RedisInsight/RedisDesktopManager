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

            property variant data: modelData || model

            iconSource: data['icon']

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: {
                if (!connectionsManager)
                    return

                if (data['callback'] != undefined) {
                    return callbacks[data['callback']]()
                }

                connectionsManager.sendEvent(styleData.index, data['event'])
            }

            tooltip: data['help'] != undefined ? data['help'] : ""
        }
    }
}
