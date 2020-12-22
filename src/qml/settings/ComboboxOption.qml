import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2

import "../common"

Item {
    id: root
    property string label
    property string description
    property string value
    property alias model: val.model

    onValueChanged: {        
        if (val.currentText != root.value) {
            val.currentIndex = val.find(root.value)
        }
    }

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1

            BetterLabel {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: root.label
            }

            Text {
                color: "grey"
                text: root.description
                visible: !!text
            }
        }

        BetterComboBox {
            id: val

            Layout.minimumWidth: 80

            onActivated: {
                if (index >= 0)
                    root.value = textAt(index)
            }

            onCountChanged: {
                if (model) {
                    currentIndex = val.find(root.value)                    
                }
            }

            Component.onCompleted: {                
                if (model) {
                    currentIndex = val.find(root.value)
                }
            }
        }
    }
}

