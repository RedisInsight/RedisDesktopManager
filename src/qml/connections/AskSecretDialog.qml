import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.3
import "./../common"

Dialog {
    id: root

    modality: Qt.ApplicationModal
    title: qsTranslate("RESP","Enter " + getSecretName()  + " to connect to ") + config.name

    property string secretId: ""
    property var config

    function getSecretName() {
       if (secretId === "ssh_password") {
           return qsTranslate("RESP","SSH Passphrase")
       } else {
           return qsTranslate("RESP","Unknown")
       }
    }

    function forceFocus() {
        secretValue.forceFocus()
    }

    contentItem: Rectangle {
        color: sysPalette.base
        implicitHeight: 100
        implicitWidth: 600

        Control {
            palette: approot.palette
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5

                RowLayout {
                    Layout.fillWidth: true

                    BetterLabel {
                        text: qsTranslate("RESP","Passphrase")
                    }
                    PasswordInput {
                        id: secretValue
                        objectName: "rdm_secret_input"

                        onTextChanged: root.config.sshPassword = text
                        onAccepted: {
                            submitSecretBtn.submit()
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 40
                    Item {
                        Layout.fillWidth: true
                    }
                    BetterButton {
                        id: submitSecretBtn

                        Layout.preferredWidth: secretValue.checkboxWidth

                        objectName: "rdm_secret_continue_btn"
                        text: qsTranslate("RESP","Continue")

                        function submit() {
                            if (!secretValue.text) {
                                return;
                            }

                            root.close()
                            connectionsManager.proceedWithConnectionSecret(root.config)
                        }

                        onClicked: submit()
                    }

                    BetterButton {
                        Layout.preferredWidth: secretValue.checkboxWidth

                        objectName: "rdm_secret_cancel_btn"
                        text: qsTranslate("RESP","Cancel")
                        onClicked: root.close()
                    }
                }
            }
        }
    }
}
