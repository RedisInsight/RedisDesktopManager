import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import "./../common"

Item {
    id: root
    property string label
    property string description
    property int value
    property alias min: val.from
    property alias max: val.to

    onValueChanged: {
        if (val.value != root.value) {
            val.value = root.value
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
                color: "#cccccc"
                text: root.description
            }
        }

        BetterSpinBox {
            id: val

            Layout.minimumWidth: 80

            onValueChanged: {
                root.value = value
            }
        }
    }
}

