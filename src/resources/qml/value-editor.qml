import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1

Item {

    anchors.fill: parent

    TabView {

        anchors.fill: parent

        Tab{
            title: "test:test:test:key"
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                RowLayout {
                    Layout.fillWidth: true

                    Text { text: "Key:" }
                    TextField { text: test; Layout.fillWidth: true}
                    Button { text: "Rename" }
                    Button { text: "Delete" }
                }

                TextEdit {
                    Component.onCompleted: {
                        forceActiveFocus()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

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

                    TableViewColumn{ role: "number"  ; title: "#" ; width: 30 }
                    TableViewColumn{ role: "key"  ; title: "Key" ; width: 100 }
                    TableViewColumn{ role: "value" ; title: "Value"; width: table.width - 130}
                }

                ColumnLayout {
                    Layout.fillWidth: true

                    TextArea {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 300
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Item { Layout.fillWidth: true}
                        Button { text: "Save"}
                    }

                }
            }
        }
    }



}
