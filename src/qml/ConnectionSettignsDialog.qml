import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.3
import "./common"
import "./common/platformutils.js" as PlatformUtils

Dialog {
    id: root
    title: isNewConnection ? qsTranslate("RDM","New Connection Settings") : qsTranslate("RDM","Edit Connection Settings") + " " + settings.name

    modality: Qt.ApplicationModal

    property bool isNewConnection: !settings || !settings.name
    property var settings
    property string quickStartGuideUrl: "http://docs.rdm.dev/en/latest/quick-start/"

    signal testConnection
    signal saveConnection(var settings)

    property var items: []
    property var sshItems: []
    property var sslItems: []
    property var sshEnabled
    property var sslEnabled

    function cleanStyle() {
        function clean(items_array) {
            for (var index=0; index < items_array.length; index++)
                if (items_array[index].validationError !== undefined)
                    items_array[index].validationError = false
        }

        clean(items)
        clean(sshItems)
        clean(sslItems)
        validationWarning.visible = false
    }

    function validate() {

        cleanStyle()

        function checkItems(items_array) {
            var errors = 0

            for (var index=0; index < items_array.length; index++) {
                var value = undefined

                if (items_array[index].text != undefined) {
                    value = items_array[index].text
                } else if (items_array[index].host != undefined) {
                    value = items_array[index].host
                } else if (items_array[index].path != undefined) {
                    value = items_array[index].path
                }

                if (value != undefined && value.length == 0) {
                    errors++
                    items_array[index].validationError = true
                }
            }

            return errors
        }

        var errors_count = checkItems(items)

        if (sshEnabled)
            errors_count += checkItems(sshItems)

        if (sslEnabled)
            errors_count += checkItems(sslItems)

        return errors_count == 0
    }

    function hideLoader() {
        uiBlocker.visible = false
    }

    function showLoader() {
        uiBlocker.visible = true
    }

    function showMsg(msg) {
        dialog_notification.showMsg(msg)
    }

    function showError(err) {
        dialog_notification.showError(err)
    }

    onVisibleChanged: {
        if (visible)
            connectionSettingsTabBar.currentIndex = 0
    }


    contentItem: Rectangle {
        color: sysPalette.base
        implicitWidth: 700
        implicitHeight: {
            if (screen.devicePixelRatio === 1) {
                return connectionSettingsTabBar.implicitHeight
                        + sshSettingsGrid.implicitHeight + 350
            } else {
                return 630
            }
        }
        Control {
            palette: approot.palette
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5

                TabBar {
                    id: connectionSettingsTabBar
                    Layout.fillWidth: true

                    palette.brightText: sysPalette.text
                    palette.dark: sysPalette.button
                    palette.mid: sysPalette.window
                    palette.window: sysPalette.base
                    palette.windowText: sysPalette.windowText

                    TabButton {
                        objectName: "rdm_connection_settings_dialog_basic_settings_tab"
                        text: qsTranslate("RDM","Connection Settings")
                    }

                    TabButton {
                        objectName: "rdm_connection_settings_dialog_advanced_settings_tab"
                        text:  qsTranslate("RDM","Advanced Settings")
                    }
                }

                StackLayout {
                    id: settingsTabs
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: connectionSettingsTabBar.currentIndex

                    BetterTab {
                        id: mainTab

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: 30
                        clip: true

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            GridLayout {
                                objectName: "rdm_connection_basic_settings"
                                columns: 2

                                Layout.fillWidth: true

                                BetterLabel { text: qsTranslate("RDM","Name:") }

                                BetterTextField {
                                    id: connectionName
                                    objectName: "rdm_connection_name_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RDM","Connection Name")
                                    text: root.settings ? root.settings.name : ""
                                    Component.onCompleted: root.items.push(connectionName)
                                    onTextChanged: root.settings.name = text
                                }

                                BetterLabel { text: qsTranslate("RDM","Address:") }

                                AddressInput {
                                    id: connectionAddress
                                    objectName: "rdm_connection_address_input"
                                    placeholderText: qsTranslate("RDM","redis-server host")
                                    host: root.settings ? root.settings.host : ""
                                    port: root.settings ? root.settings.port : 0
                                    Component.onCompleted: root.items.push(connectionAddress)
                                    onHostChanged: if (root.settings) root.settings.host = host
                                    onPortChanged: if (root.settings) root.settings.port = port
                                }

                                BetterLabel {
                                    id: windowsLocalhostWarning
                                    Layout.columnSpan: 2
                                    text: qsTranslate("RDM", "For better network performance please use 127.0.0.1")
                                    visible: !root.sshEnabled && !root.sslEnabled
                                             && String(connectionAddress.host).toLowerCase() === "localhost"
                                             && Qt.platform.os == "windows"

                                }

                                BetterLabel { text: qsTranslate("RDM","Password:") }

                                PasswordInput {
                                    id: connectionAuth
                                    objectName: "rdm_connection_auth_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RDM","(Optional) redis-server authentication password")
                                    text: root.settings ? root.settings.auth : ""
                                    onTextChanged: root.settings.auth = text
                                }

                                BetterLabel { text: qsTranslate("RDM","Username:") }

                                BetterTextField {
                                    id: connectionUsername
                                    objectName: "rdm_connection_username_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RDM","(Optional) redis-server authentication username" + " (Redis >6.0)")
                                    text: root.settings ? root.settings.username : ""
                                    onTextChanged: if (root.settings) root.settings.username = text
                                }
                            }

                            Item { Layout.preferredWidth: 10 }

                            SettingsGroupTitle { text: qsTranslate("RDM","Security") }

                            GridLayout {
                                id: securityGrid
                                objectName: "rdm_connection_group_box_security"
                                columns: 2

                                BetterRadioButton {
                                    id: sslRadioButton
                                    objectName: "rdm_connection_security_ssl_radio_button"
                                    Layout.columnSpan: 2
                                    text: qsTranslate("RDM","SSL / TLS")
                                    allowUncheck: true
                                    checked: root.settings ? root.settings.sslEnabled && !root.sshEnabled : false
                                    Component.onCompleted: root.sslEnabled = Qt.binding(function() { return sslRadioButton.checked })
                                    onCheckedChanged: {
                                        root.settings.sslEnabled = checked
                                        root.cleanStyle()
                                    }
                                }

                                Item { Layout.preferredWidth: 20 }

                                GridLayout {
                                    id: tlsSettingsGrid
                                    objectName: "rdm_connection_security_ssl_grid"
                                    enabled: sslRadioButton.checked
                                    visible: sslRadioButton.checked
                                    columns: 2
                                    Layout.fillWidth: true

                                    BetterLabel { text: qsTranslate("RDM","Public Key:") }

                                    FilePathInput {
                                        id: sslLocalCertPath
                                        objectName: "rdm_connection_security_ssl_local_cert_path_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RDM","(Optional) Public Key in PEM format")
                                        nameFilters: [ "Public Key in PEM format (*.pem *.crt)" ]
                                        title: qsTranslate("RDM","Select public key in PEM format")
                                        path: root.settings ? root.settings.sslLocalCertPath : ""
                                        onPathChanged: root.settings.sslLocalCertPath = path
                                    }

                                    BetterLabel { text: qsTranslate("RDM", "Private Key") + ":" }

                                    FilePathInput {
                                        id: sslPrivateKeyPath
                                        objectName: "rdm_connection_security_ssl_private_key_path_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RDM","(Optional) Private Key in PEM format")
                                        nameFilters: [ "Private Key in PEM format (*.pem *.key)" ]
                                        title: qsTranslate("RDM","Select private key in PEM format")
                                        path: root.settings ? root.settings.sslPrivateKeyPath : ""
                                        onPathChanged: root.settings.sslPrivateKeyPath = path
                                    }

                                    BetterLabel { text: qsTranslate("RDM","Authority:") }

                                    FilePathInput {
                                        id: sslCaCertPath
                                        objectName: "rdm_connection_security_ssl_ca_cert_path_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RDM","(Optional) Authority in PEM format")
                                        nameFilters: [ "Authority file in PEM format (*.pem *.crt)" ]
                                        title: qsTranslate("RDM","Select authority file in PEM format")
                                        path: root.settings ? root.settings.sslCaCertPath : ""
                                        onPathChanged: root.settings.sslCaCertPath = path
                                    }

                                    BetterLabel { text: qsTranslate("RDM","Enable strict mode:")}

                                    BetterCheckbox {
                                        id: ignoreSSLErrors
                                        Layout.fillWidth: true
                                        checked: root.settings ? !root.settings.ignoreSSLErrors : false
                                        onCheckedChanged: root.settings.ignoreSSLErrors = !checked
                                    }
                                }

                                BetterRadioButton {
                                    id: sshRadioButton
                                    objectName: "rdm_connection_security_ssh_radio_button"
                                    Layout.columnSpan: 2
                                    text: qsTranslate("RDM","SSH Tunnel")
                                    allowUncheck: true
                                    checked: root.settings ? root.settings.useSshTunnel() : false
                                    Component.onCompleted: root.sshEnabled = Qt.binding(function() { return sshRadioButton.checked })
                                    onCheckedChanged: {
                                        root.cleanStyle()
                                    }
                                }

                                Item { Layout.preferredWidth: 20 }

                                GridLayout {
                                    id: sshSettingsGrid
                                    objectName: "rdm_connection_security_ssh_grid"
                                    visible: sshRadioButton.checked
                                    enabled: sshRadioButton.checked
                                    columns: 2
                                    Layout.fillWidth: true

                                    BetterLabel { text: qsTranslate("RDM","SSH Address:") }

                                    AddressInput {
                                        id: sshAddress
                                        placeholderText: qsTranslate("RDM","Remote Host with SSH server")
                                        port: root.settings ? root.settings.sshPort : 22
                                        host: root.settings ? root.settings.sshHost : ""
                                        Component.onCompleted: root.sshItems.push(sshAddress)
                                        onHostChanged: root.settings.sshHost = host
                                        onPortChanged: root.settings.sshPort = port
                                    }

                                    BetterLabel { text: qsTranslate("RDM","SSH User:") }

                                    BetterTextField {
                                        id: sshUser
                                        objectName: "rdm_connection_security_ssh_user_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RDM","Valid SSH User Name")
                                        text: root.settings ? root.settings.sshUser : ""
                                        Component.onCompleted: root.sshItems.push(sshUser)
                                        onTextChanged: root.settings.sshUser = text
                                    }

                                    BetterGroupbox {
                                        labelText: qsTranslate("RDM","Private Key")
                                        objectName: "rdm_connection_security_ssh_key_group_box"
                                        checked: root.settings ? root.settings.sshPrivateKey : false

                                        Layout.columnSpan: 2
                                        Layout.fillWidth: true

                                        ColumnLayout {
                                            anchors.fill: parent

                                            FilePathInput {
                                                id: sshPrivateKey
                                                objectName: "rdm_connection_security_ssh_key_path_field"

                                                Layout.fillWidth: true

                                                placeholderText: qsTranslate("RDM","Path to Private Key in PEM format")
                                                nameFilters: [ "Private key in PEM format (*)" ]
                                                title: qsTranslate("RDM","Select private key in PEM format")
                                                path: root.settings ? root.settings.sshPrivateKey : ""
                                                onPathChanged: root.settings.sshPrivateKey = path
                                            }

                                            BetterLabel {
                                                visible: PlatformUtils.isOSX()
                                                Layout.fillWidth: true;
                                                text: qsTranslate("RDM","<b>Tip:</b> Use <code>⌘ + Shift + .</code> to show hidden files and folders in dialog") }
                                        }
                                    }

                                    BetterGroupbox {
                                        labelText: qsTranslate("RDM","Password")
                                        objectName: "rdm_connection_security_ssh_password_group_box"
                                        checked: root.settings ? root.settings.sshPassword : true

                                        Layout.columnSpan: 2
                                        Layout.fillWidth: true

                                        PasswordInput {
                                            id: sshPassword
                                            objectName: "rdm_connection_security_ssh_password_field"
                                            anchors.fill: parent
                                            placeholderText: qsTranslate("RDM","SSH User Password")
                                            text: root.settings ? root.settings.sshPassword : ""
                                            onTextChanged: root.settings.sshPassword = text
                                        }
                                    }

                                    BetterCheckbox {
                                        id: sshTLSoverSSHCheckbox
                                        objectName: "rdm_connection_security_ssh_tls_over_ssh"
                                        Layout.fillWidth: true
                                        Layout.columnSpan: 2
                                        text: qsTranslate("RDM","Enable TLS-over-SSH (<b>AWS ElastiCache</b> <b>Encryption in-transit</b>)")
                                        checked: root.settings ? root.settings.sslEnabled : false
                                        onCheckedChanged: root.settings.sslEnabled = checked

                                        Connections {
                                            target: root

                                            function onSslEnabledChanged() {
                                                // NOTE(u_glide): Workaround for case when user enables plain TLS
                                                // on existing TLS-over-SSH connection and then selects SSH again.
                                                if (!root.sslEnabled && root.settings.sshHost
                                                        && sshTLSoverSSHCheckbox.checked) {
                                                    root.settings.sslEnabled = true
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            Item { Layout.fillHeight: true }
                        }

                    }

                    BetterTab {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: 30

                        GridLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            columns: 2

                            SettingsGroupTitle {
                                text: qsTranslate("RDM","Keys loading")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RDM","Default filter:") }

                            BetterTextField
                            {
                                id: keysPattern
                                Layout.fillWidth: true
                                placeholderText: qsTranslate("RDM","Pattern which defines loaded keys from redis-server")
                                text: root.settings ? root.settings.keysPattern : ""
                                Component.onCompleted: root.items.push(keysPattern)
                                onTextChanged: if (root.settings) { root.settings.keysPattern = text }
                            }

                            BetterLabel { text: qsTranslate("RDM","Namespace Separator:") }

                            BetterTextField
                            {
                                id: namespaceSeparator
                                Layout.fillWidth: true
                                objectName: "rdm_advanced_settings_namespace_separator_field"
                                placeholderText: qsTranslate("RDM","Separator used for namespace extraction from keys")
                                text: root.settings ? root.settings.namespaceSeparator : ""
                                onTextChanged: if (root.settings) { root.settings.namespaceSeparator = text }
                            }

                            SettingsGroupTitle {
                                text: qsTranslate("RDM","Timeouts & Limits")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RDM","Connection Timeout (sec):") }

                            BetterSpinBox {
                                id: executeTimeout
                                Layout.fillWidth: true
                                from: 10
                                to: 100000
                                value: {
                                    return root.settings ? (root.settings.executeTimeout / 1000.0) : 0
                                }
                                onValueChanged: if (root.settings) { root.settings.executeTimeout = value * 1000 }
                            }

                            BetterLabel { text: qsTranslate("RDM","Execution Timeout (sec):")}

                            BetterSpinBox {
                                id: connectionTimeout
                                Layout.fillWidth: true
                                from: 10
                                to: 100000
                                value: root.settings ? (root.settings.connectionTimeout / 1000.0) : 0
                                onValueChanged: if (root.settings) { root.settings.connectionTimeout = value * 1000 }
                            }

                            BetterLabel { text: qsTranslate("RDM","Databases discovery limit:") }

                            BetterSpinBox {
                                id: dbScanLimit
                                Layout.fillWidth: true
                                from: 1
                                to: 100000
                                value: {
                                    return root.settings ? root.settings.databaseScanLimit : 1
                                }
                                onValueChanged: if (root.settings) { root.settings.databaseScanLimit = value }
                            }

                            SettingsGroupTitle {
                                text: qsTranslate("RDM","Cluster")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RDM","Change host on cluster redirects:")}

                            BetterCheckbox {
                                id: overrideClusterHost
                                Layout.fillWidth: true
                                checked: root.settings ? root.settings.overrideClusterHost : false
                                onCheckedChanged: if (root.settings) { root.settings.overrideClusterHost = checked }
                            }

                            Item {
                                Layout.columnSpan: 2
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30

                    BetterButton {
                        objectName: "rdm_connection_settings_dialog_test_btn"
                        iconSource: "qrc:/images/offline.svg"
                        text: qsTranslate("RDM","Test Connection")
                        onClicked: {
                            showLoader()
                            root.testConnection(root.settings)
                        }
                    }

                    BetterButton {
                        iconSource: "qrc:/images/help.svg"
                        text: qsTranslate("RDM","Quick Start Guide")
                        onClicked: Qt.openUrlExternally(root.quickStartGuideUrl)
                    }

                    Item { Layout.fillWidth: true }

                    RowLayout {
                        id: validationWarning
                        visible: false
                        Layout.fillWidth: true

                        Image {
                            width: 15
                            height: 15
                            sourceSize.width: 30
                            sourceSize.height: 30
                            source: "qrc:/images/alert.svg"
                        }
                        BetterLabel {
                            text: qsTranslate("RDM","Invalid settings detected!")
                        }
                    }

                    Item { Layout.fillWidth: true }

                    BetterButton {
                        objectName: "rdm_connection_settings_dialog_ok_btn"
                        text: qsTranslate("RDM","OK")
                        onClicked: {
                            if (root.validate()) {
                                root.saveConnection(root.settings)
                                root.settings = connectionsManager.createEmptyConfig()
                                root.close()
                            } else {
                                validationWarning.visible = true
                            }
                        }
                    }

                    BetterButton {
                        objectName: "rdm_connection_settings_dialog_cancel_btn"
                        text: qsTranslate("RDM","Cancel")
                        onClicked: {
                            root.settings = connectionsManager.createEmptyConfig()
                            root.close()
                        }
                    }
                }
            }

            Rectangle {
                id: uiBlocker
                visible: false
                anchors.fill: parent
                color: Qt.rgba(0, 0, 0, 0.1)

                Item {
                    anchors.fill: parent
                    BusyIndicator { anchors.centerIn: parent; running: true }
                }

                MouseArea {
                    anchors.fill: parent
                }
            }
            MessageDialog {
                id: dialog_notification
                objectName: "rdm_qml_connection_settings_error_dialog"
                visible: false
                modality: Qt.NonModal
                icon: StandardIcon.Warning
                standardButtons: StandardButton.Ok

                function showError(msg) {
                    icon = StandardIcon.Warning
                    text = msg
                    open()
                }

                function showMsg(msg) {
                    icon = StandardIcon.Information
                    text = msg
                    open()
                }
            }
        }
    }
}
