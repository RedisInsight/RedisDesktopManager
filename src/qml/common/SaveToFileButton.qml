import QtQuick 2.0
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

ImageButton {
    id: root
    iconSource: "qrc:/images/document.svg"
    tooltip: qsTranslate("RDM","Save to File")
    property string path

    onClicked: {
        saveValueToFileDialog.open()
    }

    FileDialog {
        id: saveValueToFileDialog
        title: qsTranslate("RDM","Save Value")
        nameFilters: ["All files (*)"]
        selectExisting: false

        onAccepted: {
            root.path = qmlUtils.getPathFromUrl(fileUrl)
            if (qmlUtils.saveToFile(value, root.path)) {
                saveToFileConfirmation.open()
            }
        }
    }

    Dialog {
        id: saveToFileConfirmation
        title: qsTranslate("RDM","Save value to file")
        visible: false

        contentItem: Rectangle {
            color: sysPalette.base
            anchors.fill: parent

            implicitWidth: 500
            implicitHeight: 150

            Control {
                palette: approot.palette
                anchors.fill: parent
                anchors.margins: 15

                ColumnLayout {
                    anchors.fill: parent

                    Label {
                        Layout.fillWidth: true
                        text: qsTranslate("RDM","Value was saved to file:")
                    }

                    TextEdit {
                        Layout.fillWidth: true
                        text: root.path
                        color: sysPalette.text
                        readOnly: true
                        selectByMouse: true
                        wrapMode: Text.Wrap
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        Control {
                            RowLayout {
                                Layout.fillHeight: true

                                RichTextWithLinks {
                                    html: "<a href='file://" + root.path + "'>Open File</a>"
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillHeight: true

                        Item { Layout.fillWidth: true; }

                        BetterButton {
                            text: qsTranslate("RDM","OK")
                            onClicked: saveToFileConfirmation.close()
                        }
                    }
                }
            }
        }
    }
}
