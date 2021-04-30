import QtQuick 2.0
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

ImageButton {
    id: root
    iconSource: raw ? "qrc:/images/binary_file.svg" : "qrc:/images/code_file.svg"
    tooltip: raw ? qsTranslate("RDM","Save Raw Value to File") : qsTranslate("RDM","Save Formatted Value to File") + " (" + shortcutText + ")"

    property string fileUrl
    property string folderUrl
    property string path
    property string shortcutText: ""
    property bool raw: false


    onClicked: saveToFile()

    function saveToFile() {
        saveValueToFileDialog.open()
    }

    FileDialog {
        id: saveValueToFileDialog
        title: raw ? qsTranslate("RDM","Save Raw Value") : qsTranslate("RDM","Save Formatted Value")
        nameFilters: ["All files (*)"]
        selectExisting: false

        onAccepted: {
            root.fileUrl = fileUrl

            var path = qmlUtils.getPathFromUrl(fileUrl)
            root.folderUrl = qmlUtils.getUrlFromPath(qmlUtils.getDir(path))
            root.path = qmlUtils.getNativePath(path)
            if (raw) {
                if (qmlUtils.saveToFile(value, root.path)) {
                    saveToFileConfirmation.open()
                }
            } else {
                if (qmlUtils.saveToFile(textView.model.getText(), root.path)) {
                    saveToFileConfirmation.open()
                }
            }
        }
    }

    Dialog {
        id: saveToFileConfirmation
        title: raw ? qsTranslate("RDM","Save raw value to file") : qsTranslate("RDM","Save formatted value to file")
        visible: false

        contentItem: Rectangle {
            objectName: "rdm_save_to_file_confirmation_dialog"
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

                    BetterLabel {
                        Layout.fillWidth: true
                        text: qsTranslate("RDM","Value was saved to file:")
                    }

                    TextEdit {
                        objectName: "rdm_save_to_file_confirmation_dialog_path"
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
                                spacing: 15

                                RichTextWithLinks {
                                    Layout.fillWidth: true
                                    wrapMode: Text.NoWrap
                                    html: "<a href='" + root.fileUrl + "'>Open File</a>"
                                }

                                RichTextWithLinks {
                                    Layout.fillWidth: true
                                    wrapMode: Text.NoWrap
                                    html: "<a href='" + root.folderUrl + "'>Open Folder</a>"
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillHeight: true

                        Item { Layout.fillWidth: true; }

                        BetterButton {
                            objectName: "rdm_save_to_file_confirmation_dialog_ok_btn"
                            text: qsTranslate("RDM","OK")
                            onClicked: saveToFileConfirmation.close()
                        }
                    }
                }
            }
        }
    }
}
