import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3
import "./common"
import "./common/platformutils.js" as PlatformUtils

Dialog {
    id: root    
    title: !settings || !settings.name ? qsTr("New Connection Settings") : qsTr("Edit Connection Settings - %1").arg(settings.name)

    property var settings
    property string quickStartGuideUrl: "http://docs.redisdesktop.com/en/latest/quick-start/"

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
                if (items_array[index].enabled)
                    items_array[index].style = validStyleEnabled.style
                else
                    items_array[index].style = validStyleDisabled.style
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
                    items_array[index].style = invalidStyle
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
            settingsTabs.currentIndex = 0
    }

    Component {
        id: invalidStyle

        TextFieldStyle {
            textColor: "red"
            background: Rectangle {
                radius: 2
                implicitWidth: 100
                implicitHeight: 24
                border.color: "red"
                border.width: 1
            }
        }
    }

    TextField { id: validStyleEnabled; visible: false}
    TextField { id: validStyleDisabled; visible: false; enabled: false}

    contentItem: Item {
        implicitWidth: 600
        implicitHeight: PlatformUtils.isOSX()? 650 : Math.min(750, Screen.desktopAvailableHeight - 100)

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            TabView {
                id: settingsTabs
                Layout.fillWidth: true
                Layout.fillHeight: true                

                Tab {
                    id: mainTab
                    title: qsTr("Connection Settings")

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: PlatformUtils.isOSX()? 5 : 10

                        SettingsGroupTitle { text: qsTr("Main Settings") }

                        GridLayout {
                            columns: 2

                            Label { text: qsTr("Name:") }

                            TextField {
                                id: connectionName
                                objectName: "rdm_connection_name_field"
                                Layout.fillWidth: true
                                placeholderText: qsTr("Connection Name")
                                text: root.settings ? root.settings.name : ""
                                Component.onCompleted: root.items.push(connectionName)
                                onTextChanged: root.settings.name = text
                            }

                            Label { text: qsTr("Address:") }

                            AddressInput {
                                id: connectionAddress
                                placeholderText: qsTr("redis-server host")
                                host: root.settings ? root.settings.host : ""
                                port: root.settings ? root.settings.port : 0
                                Component.onCompleted: root.items.push(connectionAddress)
                                onHostChanged: if (root.settings) root.settings.host = host
                                onPortChanged: if (root.settings) root.settings.port = port
                            }

                            Label { text: qsTr("Auth:") }

                            PasswordInput {
                                id: connectionAuth
                                Layout.fillWidth: true
                                placeholderText: qsTr("(Optional) redis-server authentication password")
                                text: root.settings ? root.settings.auth : ""
                                onTextChanged: root.settings.auth = text
                            }
                        }

                        SettingsGroupTitle { text: qsTr("Security") }

                        GridLayout {
                            columns: 2

                            BetterRadioButton {
                                text: qsTr("None")
                                checked: root.settings ? !root.settings.sslEnabled && !root.settings.useSshTunnel() : true
                                Layout.columnSpan: 2
                            }

                            BetterRadioButton {
                                id: sslRadioButton
                                Layout.columnSpan: 2
                                text: qsTr("SSL")
                                checked: root.settings ? root.settings.sslEnabled : false
                                Component.onCompleted: root.sslEnabled = Qt.binding(function() { return sslRadioButton.checked })
                                onCheckedChanged: {
                                    root.settings.sslEnabled = checked
                                    root.cleanStyle()

                                    if (!checked) {
                                        sslLocalCertPath.path = ""
                                        sslPrivateKeyPath.path = ""
                                        sslCaCertPath.path = ""
                                    }
                                }
                            }

                            Item { Layout.preferredWidth: 20 }

                            GridLayout {
                                enabled: sslRadioButton.checked
                                columns: 2
                                Layout.fillWidth: true

                                Label { text: qsTr("Public Key:") }

                                FilePathInput {
                                    id: sslLocalCertPath
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("(Optional) Public Key in PEM format")
                                    nameFilters: [ "Public Key in PEM format (*.pem *.crt)" ]
                                    title: qsTr("Select public key in PEM format")
                                    path: root.settings ? root.settings.sslLocalCertPath : ""
                                    onPathChanged: root.settings.sslLocalCertPath = path
                                }

                                Label { text: qsTr("Private Key:") }

                                FilePathInput {
                                    id: sslPrivateKeyPath
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("(Optional) Private Key in PEM format")
                                    nameFilters: [ "Private Key in PEM format (*.pem *.key)" ]
                                    title: qsTr("Select private key in PEM format")
                                    path: root.settings ? root.settings.sslPrivateKeyPath : ""
                                    onPathChanged: root.settings.sslPrivateKeyPath = path
                                }

                                Label { text: qsTr("Authority:") }

                                FilePathInput {
                                    id: sslCaCertPath
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("(Optional) Authority in PEM format")
                                    nameFilters: [ "Authority file in PEM format (*.pem *.crt)" ]
                                    title: qsTr("Select authority file in PEM format")
                                    path: root.settings ? root.settings.sslCaCertPath : ""
                                    onPathChanged: root.settings.sslCaCertPath = path
                                }
                            }

                            BetterRadioButton {
                                id: sshRadioButton
                                objectName: "rdm_connection_security_ssh_radio_button"
                                Layout.columnSpan: 2
                                text: qsTr("SSH Tunnel")
                                checked: root.settings ? root.settings.useSshTunnel() : false
                                Component.onCompleted: root.sshEnabled = Qt.binding(function() { return sshRadioButton.checked })
                                onCheckedChanged: {
                                    root.cleanStyle()

                                    if (!checked) {
                                        sshAddress.host = ""
                                        sshAddress.port = 22
                                        sshUser.text = ""
                                        sshPrivateKey.path = ""
                                        sshPassword.text = ""
                                    }
                                }
                            }

                            Item { Layout.preferredWidth: 20 }

                            GridLayout {
                                enabled: sshRadioButton.checked
                                columns: 2
                                Layout.fillWidth: true

                                Label { text: qsTr("SSH Address:") }

                                AddressInput {
                                    id: sshAddress
                                    placeholderText: qsTr("Remote Host with SSH server")
                                    port: root.settings ? root.settings.sshPort : 22
                                    host: root.settings ? root.settings.sshHost : ""
                                    Component.onCompleted: root.sshItems.push(sshAddress)
                                    onHostChanged: root.settings.sshHost = host
                                    onPortChanged: root.settings.sshPort = port
                                }

                                Label { text: qsTr("SSH User:") }

                                TextField {
                                    id: sshUser
                                    objectName: "rdm_connection_security_ssh_user_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("Valid SSH User Name")
                                    text: root.settings ? root.settings.sshUser : ""
                                    Component.onCompleted: root.sshItems.push(sshUser)
                                    onTextChanged: root.settings.sshUser = text
                                }

                                BetterGroupbox {
                                    labelText: qsTr("Private Key")
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

                                            placeholderText: qsTr("Path to Private Key in PEM format")
                                            nameFilters: [ "Private key in PEM format (*)" ]
                                            title: qsTr("Select private key in PEM format")
                                            path: root.settings ? root.settings.sshPrivateKey : ""
                                            onPathChanged: root.settings.sshPrivateKey = path
                                        }

                                        Label {
                                            visible: PlatformUtils.isOSX()
                                            Layout.fillWidth: true;
                                            text: qsTr("<b>Tip:</b> Use <code>⌘ + Shift + .</code> to show hidden files and folders in dialog") }
                                    }
                                }

                                BetterGroupbox {
                                    labelText: qsTr("Password")
                                    objectName: "rdm_connection_security_ssh_password_group_box"
                                    checked: root.settings ? root.settings.sshPassword : true

                                    Layout.columnSpan: 2
                                    Layout.fillWidth: true

                                    PasswordInput {
                                        id: sshPassword
                                        objectName: "rdm_connection_security_ssh_password_field"
                                        anchors.fill: parent
                                        placeholderText: qsTr("SSH User Password")
                                        text: root.settings ? root.settings.sshPassword : ""
                                        onTextChanged: root.settings.sshPassword = text
                                    }
                                }
                            }
                        }

                    }
                }

                Tab {
                    title: qsTr("Advanced Settings")

                       GridLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            columns: 2

                            SettingsGroupTitle {
                                text: qsTr("Keys loading")                                
                                Layout.columnSpan: 2
                            }

                            Label { text: qsTr("Default filter:") }

                            TextField
                            {
                                id: keysPattern
                                Layout.fillWidth: true
                                placeholderText: qsTr("Pattern which defines loaded keys from redis-server")
                                text: root.settings ? root.settings.keysPattern : ""
                                Component.onCompleted: root.items.push(keysPattern)
                                onTextChanged: root.settings.keysPattern = text
                            }

                            Label { text: qsTr("Namespace Separator:") }

                            TextField
                            {
                                id: namespaceSeparator
                                Layout.fillWidth: true
                                objectName: "rdm_advanced_settings_namespace_separator_field"
                                placeholderText: qsTr("Separator used for namespace extraction from keys")
                                text: root.settings ? root.settings.namespaceSeparator : ""
                                onTextChanged: root.settings.namespaceSeparator = text
                            }

                            Label { text: qsTr("Use server-side optimized keys loading (experimental):")}

                            BetterCheckbox {
                                id: luaKeysLoading
                                Layout.fillWidth: true
                                checked: root.settings ? (root.settings.luaKeysLoading / 1000.0) : true
                                onCheckedChanged: root.settings.luaKeysLoading = checked

                            }

                            SettingsGroupTitle {
                                text: qsTr("Timeouts & Limits")                               
                                Layout.columnSpan: 2
                            }

                            Label { text: qsTr("Connection Timeout (sec):") }

                            SpinBox {
                                id: executeTimeout
                                Layout.fillWidth: true
                                minimumValue: 30
                                maximumValue: 100000
                                value: {
                                    return root.settings ? (root.settings.executeTimeout / 1000.0) : 0
                                }
                                onValueChanged: root.settings.executeTimeout = value * 1000
                            }

                            Label { text: qsTr("Execution Timeout (sec):")}

                            SpinBox {
                                id: connectionTimeout
                                Layout.fillWidth: true
                                minimumValue: 30
                                maximumValue: 100000
                                value: root.settings ? (root.settings.connectionTimeout / 1000.0) : 0
                                onValueChanged: root.settings.connectionTimeout = value * 1000
                            }

                            Label { text: qsTr("Databases discovery limit:") }

                            SpinBox {
                                id: dbScanLimit
                                Layout.fillWidth: true
                                minimumValue: 1
                                maximumValue: 100000
                                value: {
                                    return root.settings ? root.settings.databaseScanLimit : 1
                                }
                                onValueChanged: root.settings.databaseScanLimit = value
                            }

                            SettingsGroupTitle {
                                text: qsTr("Cluster")                                
                                Layout.columnSpan: 2
                            }

                            Label { text: qsTr("Change host on cluster redirects:")}

                            BetterCheckbox {
                                id: overrideClusterHost
                                Layout.fillWidth: true
                                checked: root.settings ? root.settings.overrideClusterHost : false
                                onCheckedChanged: root.settings.overrideClusterHost = checked
                            }

                            Item {
                                Layout.columnSpan: 2
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }
                }
            }

            Item {
                id: validationWarning
                visible: false
                Layout.fillWidth: true

                implicitHeight: 25

                RowLayout {
                    anchors.centerIn: parent
                    Image {source: "qrc:/images/alert.svg"}
                    Text { text: qsTr("Invalid settings detected!")}
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    objectName: "rdm_connection_settings_dialog_test_btn"
                    iconSource: "qrc:/images/offline.svg"
                    text: qsTr("Test Connection")
                    onClicked: {
                        showLoader()
                        root.testConnection(root.settings)
                    }
                }

                ImageButton {
                    Layout.preferredWidth: 25
                    Layout.preferredHeight: 25
                    imgSource: "qrc:/images/help.svg"
                    imgHeight: 30
                    imgWidth: 30
                    onClicked: Qt.openUrlExternally(root.quickStartGuideUrl)
                }

                Item { Layout.fillWidth: true }

                Button {
                    objectName: "rdm_connection_settings_dialog_ok_btn"
                    text: qsTr("OK")
                    onClicked: {
                        if (root.validate()) {                            
                            root.saveConnection(root.settings)
                            root.close()
                        } else {
                            validationWarning.visible = true
                        }
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    onClicked: root.close()
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
            modality: Qt.WindowModal
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
