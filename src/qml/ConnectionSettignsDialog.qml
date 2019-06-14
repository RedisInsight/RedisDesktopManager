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
    title: isNewConnection ? qsTranslate("RDM","New Connection Settings") : qsTranslate("RDM","Edit Connection Settings") + settings.name

    modality: Qt.ApplicationModal

    property bool isNewConnection: !settings || !settings.name
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
        implicitWidth: 800
        implicitHeight: PlatformUtils.isOSX()? 650 : 600

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            TabView {
                id: settingsTabs
                Layout.fillWidth: true
                Layout.fillHeight: true                

                Tab {
                    id: mainTab
                    anchors.fill: parent
                    anchors.margins: 10
                    title: qsTranslate("RDM","Connection Settings")

                    ScrollView {
                        id: mainSettingsScrollView
                        width: settingsTabs.width - 20
                        height: settingsTabs.height

                        ColumnLayout {                                                        
                            width: mainSettingsScrollView.width
                            height: children.height

                            SettingsGroupTitle { text: qsTranslate("RDM","Main Settings") }

                            GridLayout {
                                columns: 2

                                Layout.fillWidth: true

                                Label { text: qsTranslate("RDM","Name:") }

                                TextField {
                                    id: connectionName
                                    objectName: "rdm_connection_name_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RDM","Connection Name")
                                    text: root.settings ? root.settings.name : ""
                                    Component.onCompleted: root.items.push(connectionName)
                                    onTextChanged: root.settings.name = text
                                }

                                Label { text: qsTranslate("RDM","Address:") }

                                AddressInput {
                                    id: connectionAddress
                                    placeholderText: qsTranslate("RDM","redis-server host")
                                    host: root.settings ? root.settings.host : ""
                                    port: root.settings ? root.settings.port : 0
                                    Component.onCompleted: root.items.push(connectionAddress)
                                    onHostChanged: if (root.settings) root.settings.host = host
                                    onPortChanged: if (root.settings) root.settings.port = port
                                }

                                Label { text: qsTranslate("RDM","Auth:") }

                                PasswordInput {
                                    id: connectionAuth
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RDM","(Optional) redis-server authentication password")
                                    text: root.settings ? root.settings.auth : ""
                                    onTextChanged: root.settings.auth = text
                                }
                            }

                            SettingsGroupTitle { text: qsTranslate("RDM","Security") }

                            GridLayout {
                                objectName: "rdm_connection_group_box_security"
                                columns: 2

                                BetterRadioButton {
                                    text: qsTranslate("RDM","None")
                                    checked: root.settings ? !root.settings.sslEnabled && !root.settings.useSshTunnel() : true
                                    Layout.columnSpan: 2
                                }

                                BetterRadioButton {
                                    id: sslRadioButton
                                    Layout.columnSpan: 2
                                    text: qsTranslate("RDM","SSL")
                                    checked: root.settings ? root.settings.sslEnabled && !root.sshEnabled : false
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

                                    Label { text: qsTranslate("RDM","Public Key:") }

                                    FilePathInput {
                                        id: sslLocalCertPath
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RDM","(Optional) Public Key in PEM format")
                                        nameFilters: [ "Public Key in PEM format (*.pem *.crt)" ]
                                        title: qsTranslate("RDM","Select public key in PEM format")
                                        path: root.settings ? root.settings.sslLocalCertPath : ""
                                        onPathChanged: root.settings.sslLocalCertPath = path
                                    }

                                    Label { text: qsTranslate("RDM", "Private Key") + ":" }

                                    FilePathInput {
                                        id: sslPrivateKeyPath
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RDM","(Optional) Private Key in PEM format")
                                        nameFilters: [ "Private Key in PEM format (*.pem *.key)" ]
                                        title: qsTranslate("RDM","Select private key in PEM format")
                                        path: root.settings ? root.settings.sslPrivateKeyPath : ""
                                        onPathChanged: root.settings.sslPrivateKeyPath = path
                                    }

                                    Label { text: qsTranslate("RDM","Authority:") }

                                    FilePathInput {
                                        id: sslCaCertPath
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RDM","(Optional) Authority in PEM format")
                                        nameFilters: [ "Authority file in PEM format (*.pem *.crt)" ]
                                        title: qsTranslate("RDM","Select authority file in PEM format")
                                        path: root.settings ? root.settings.sslCaCertPath : ""
                                        onPathChanged: root.settings.sslCaCertPath = path
                                    }
                                }

                                BetterRadioButton {
                                    id: sshRadioButton
                                    objectName: "rdm_connection_security_ssh_radio_button"
                                    Layout.columnSpan: 2
                                    text: qsTranslate("RDM","SSH Tunnel")
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

                                    Label { text: qsTranslate("RDM","SSH Address:") }

                                    AddressInput {
                                        id: sshAddress
                                        placeholderText: qsTranslate("RDM","Remote Host with SSH server")
                                        port: root.settings ? root.settings.sshPort : 22
                                        host: root.settings ? root.settings.sshHost : ""
                                        Component.onCompleted: root.sshItems.push(sshAddress)
                                        onHostChanged: root.settings.sshHost = host
                                        onPortChanged: root.settings.sshPort = port
                                    }

                                    Label { text: qsTranslate("RDM","SSH User:") }

                                    TextField {
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

                                            Label {
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
                                        checked: root.settings ? root.settings.sslEnabled && root.sshEnabled : false
                                        onCheckedChanged: root.settings.sslEnabled = checked
                                    }
                                }
                            }

                        }
                    }
                }

                Tab {
                    title: qsTranslate("RDM","Advanced Settings")

                       GridLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            columns: 2

                            SettingsGroupTitle {
                                text: qsTranslate("RDM","Keys loading")                                
                                Layout.columnSpan: 2
                            }

                            Label { text: qsTranslate("RDM","Default filter:") }

                            TextField
                            {
                                id: keysPattern
                                Layout.fillWidth: true
                                placeholderText: qsTranslate("RDM","Pattern which defines loaded keys from redis-server")
                                text: root.settings ? root.settings.keysPattern : ""
                                Component.onCompleted: root.items.push(keysPattern)
                                onTextChanged: root.settings.keysPattern = text
                            }

                            Label { text: qsTranslate("RDM","Namespace Separator:") }

                            TextField
                            {
                                id: namespaceSeparator
                                Layout.fillWidth: true
                                objectName: "rdm_advanced_settings_namespace_separator_field"
                                placeholderText: qsTranslate("RDM","Separator used for namespace extraction from keys")
                                text: root.settings ? root.settings.namespaceSeparator : ""
                                onTextChanged: root.settings.namespaceSeparator = text
                            }

                            Label { text: qsTranslate("RDM","Use server-side optimized keys loading (experimental):")}

                            BetterCheckbox {
                                id: luaKeysLoading
                                Layout.fillWidth: true
                                checked: root.settings ? (root.settings.luaKeysLoading / 1000.0) : true
                                onCheckedChanged: root.settings.luaKeysLoading = checked

                            }

                            SettingsGroupTitle {
                                text: qsTranslate("RDM","Timeouts & Limits")                               
                                Layout.columnSpan: 2
                            }

                            Label { text: qsTranslate("RDM","Connection Timeout (sec):") }

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

                            Label { text: qsTranslate("RDM","Execution Timeout (sec):")}

                            SpinBox {
                                id: connectionTimeout
                                Layout.fillWidth: true
                                minimumValue: 30
                                maximumValue: 100000
                                value: root.settings ? (root.settings.connectionTimeout / 1000.0) : 0
                                onValueChanged: root.settings.connectionTimeout = value * 1000
                            }

                            Label { text: qsTranslate("RDM","Databases discovery limit:") }

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
                                text: qsTranslate("RDM","Cluster")                                
                                Layout.columnSpan: 2
                            }

                            Label { text: qsTranslate("RDM","Change host on cluster redirects:")}

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
                    Text { text: qsTranslate("RDM","Invalid settings detected!")}
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    objectName: "rdm_connection_settings_dialog_test_btn"
                    iconSource: "qrc:/images/offline.svg"
                    text: qsTranslate("RDM","Test Connection")
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
                    text: qsTranslate("RDM","OK")
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
                    text: qsTranslate("RDM","Cancel")
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
