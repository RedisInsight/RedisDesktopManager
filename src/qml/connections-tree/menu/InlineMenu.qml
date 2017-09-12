import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "./../../"

RowLayout {
   id: root
   property alias model: repeater.model
   property var callbacks

   function sendEvent(e) {
       if (!connectionsManager)
           return

       connectionsManager.sendEvent(styleData.index, e)
   }

   function callCallback(c) {
       return callbacks[c]()
   }

   Repeater {
        id: repeater

        ToolButton {

            property variant data: modelData || model

            iconSource: data['icon']

            Layout.preferredWidth: 25
            Layout.preferredHeight: 25

            onClicked: {                
                if (data['callback'] != undefined)
                    return root.callCallback(data['callback'])
                else
                    return root.sendEvent(data['event'])
            }

            tooltip: data['help'] != undefined ? data['help'] : ""
            objectName: data['event'] != undefined ? "rdm_server_menu_button_" + data['event'] : ""
        }
    }
}
