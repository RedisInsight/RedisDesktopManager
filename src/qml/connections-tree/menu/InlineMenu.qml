import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "./../../common/platformutils.js" as PlatformUtils
import "./../../"
import "./../../common/"

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

        Item {
            Layout.preferredWidth: PlatformUtils.isOSXRetina(Screen)? 20 : 25
            Layout.preferredHeight: PlatformUtils.isOSXRetina(Screen)? 20 : 25
            Layout.maximumHeight: PlatformUtils.isOSXRetina(Screen)? 20 : 25

            ImageButton {
                id: actionButton
                anchors.fill: parent

                iconSource: modelData['icon']
                imgWidth: PlatformUtils.isOSXRetina(Screen)? 20 : 25
                imgHeight: PlatformUtils.isOSXRetina(Screen)? 20 : 25

                onClicked: handleClick()

                function handleClick() {
                    if (modelData['callback'] != undefined)
                        return root.callCallback(modelData['callback'])
                    else
                        return root.sendEvent(modelData['event'])
                }

                tooltip: modelData['help'] != undefined ? modelData['help'] + (modelData["shortcut"]? " (" + modelData["shortcut"] + ")" : "")  : ""

                objectName: {
                    if (modelData['event'] != undefined)
                        return "rdm_inline_menu_button_" + modelData['event']

                    if (modelData['callback'] != undefined)
                        return "rdm_inline_menu_button_" + modelData['callback']

                     return ""
                }
            }

            Shortcut {
                sequence: modelData["shortcut"]
                onActivated: actionButton.handleClick()
            }
        }
    }
}
