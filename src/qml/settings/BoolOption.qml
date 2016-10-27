import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Item {
    id: root
    property string label
    property string description
    property bool value

    onValueChanged: {
        if (val.checked != root.value) {
            val.checked = root.value
        }
    }

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1

            Label {
                Layout.fillWidth: true
                text: root.label
            }

            Text {
                color: "grey"
                text: root.description
            }
        }

        Switch {
            id: val

            onCheckedChanged: {
                root.value = checked
            }
        }
    }
}

