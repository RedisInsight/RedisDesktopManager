import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

Repeater {

    Tab {
        title: keyName
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
                TextField { Layout.fillWidth: true; text: keyName}

                Button {
                    text: "Rename"

                    MessageDialog {
                        id: renameConfirmation
                        title: "Rename key"
                        text: "Do you really want to rename this key?"
                        onAccepted: {
                            // TODO
                        }
                        visible: false
                        modality: Qt.WindowModal
                        icon: StandardIcon.Warning
                        standardButtons: StandardButton.Yes | StandardButton.No
                    }

                    onClicked: {
                        renameConfirmation.open()
                    }
                }

                Button { text: "Reload" }

                Button {
                    text: "Delete"


                    MessageDialog {
                        id: deleteConfirmation
                        title: "Delete key"
                        text: "Do you really want to delete this key?"
                        onAccepted: {
                            // TODO
                        }
                        visible: false
                        modality: Qt.WindowModal
                        icon: StandardIcon.Warning
                        standardButtons: StandardButton.Yes | StandardButton.No
                    }

                    onClicked: {
                        deleteConfirmation.open()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40

                Text { text: "Type:"; font.bold: true }
                Text { text: keyType.toUpperCase()  }
                Text { text: "TTL:"; font.bold: true }
                Text { text: keyTtl}
            }

            RowLayout {
                id: pagination
                visible: showValueNavigation
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.minimumHeight: 40

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
                visible: showValueNavigation
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 100

                TableViewColumn{ role: "number"  ; title: "#" ; width: 30 }
                TableViewColumn{ role: "key"  ; title: "Key" ; width: 100 }
                TableViewColumn{ role: "value" ; title: "Value"; width: table.width - 130}
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: !showValueNavigation

                TextArea {
                    Layout.fillWidth: true
                    Layout.fillHeight: !showValueNavigation
                    text: {
                        if (keyType === "string") {

                        }
                    }
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
