import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1

Repeater {

    Tab {
        title: name // "test:test:test:key"
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 1

            RowLayout {
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40
                Layout.fillWidth: true
                spacing: 1

                Text { text: "Key:" }
                TextField { Layout.fillWidth: true; text: name}
                Button { text: "Rename" }
                Button { text: "Reload" }
                Button { text: "Delete" }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40

                Text { text: "Type:"; font.bold: true }
                Text { text: "ZSET"  }
                Text { text: "TTL:"; font.bold: true }
                Text { text: "100012"}

                //pagination
                Button { text: "|<"}
                Button { text: "<"}
                TextField { text: "1"; Layout.fillWidth: true; readOnly: false}
                Button {
                    text: "Goto Index"
                }
                Button { text: ">"}
                Button { text: ">|"}
            }

            TableView {
                id: table
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 100

                TableViewColumn{ role: "number"  ; title: "#" ; width: 30 }
                TableViewColumn{ role: "key"  ; title: "Key" ; width: 100 }
                TableViewColumn{ role: "value" ; title: "Value"; width: table.width - 130}
            }

            ColumnLayout {
                Layout.fillWidth: true

                TextArea {
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 40
                    Item { Layout.fillWidth: true}
                    Button { text: "Save"}
                }

            }
        }
    }
}
