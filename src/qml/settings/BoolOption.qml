import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import "../common"

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

    signal clicked

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1

            BetterLabel {
                Layout.fillWidth: true
                text: root.label
            }

            Text {
                color: disabledSysPalette.text
                text: root.description
                visible: root.description
            }
        }

        Switch {
            id: val
            rightPadding: 0

            indicator: Rectangle {
                    implicitWidth: 48
                    implicitHeight: 26
                    x: val.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 13
                    color: val.checked ? sysPalette.highlight : sysPalette.button
                    border.color: val.checked ? sysPalette.highlight : sysPalette.button

                    Rectangle {
                        x: val.checked ? parent.width - width : 0
                        width: 26
                        height: 26
                        radius: 13
                        color: val.checked ? sysPalette.midlight : sysPalette.mid
                        border.color: val.checked ? sysPalette.highlight : sysPalette.button
                    }
                }

            onCheckedChanged: {
                root.value = checked                
            }

            onClicked: {
                root.clicked()
            }
        }
    }
}

