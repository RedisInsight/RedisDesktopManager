import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtQuick.Window 2.3
import "../common"
import "../common/platformutils.js" as PlatformUtils

BetterDialog {
    id: root
    title: isNewConnection ? qsTranslate("RESP","New Connection Settings") : qsTranslate("RESP","Edit Connection Settings") + " " + settings.name

    footer: null

    property bool isNewConnection: !settings || !settings.name
    property var settings
    property string quickStartGuideUrl: "http://docs.resp.app/en/latest/quick-start/"

    signal testConnection
    signal saveConnection(var settings)
    signal resetSettings

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

    function isConnectionStringValid(connectionString) {
        return connectionsManager.isRedisConnectionStringValid(connectionString)
    }

    function parseConnectionString(connectionString) {
        return connectionsManager.parseConfigFromRedisConnectionString(connectionString)
    }

    onVisibleChanged: {
        if (visible) {
            connectionSettingsTabBar.currentIndex = isNewConnection ? 0 : 1
            if (isNewConnection) {
                connectionStringField.forceActiveFocus()
            }
        }
    }

    contentItem: Rectangle {
        color: sysPalette.base
        implicitWidth: PlatformUtils.isScalingDisabled() ? 900 : 650
        implicitHeight: {
            if (screen.devicePixelRatio === 1) {
                return connectionSettingsTabBar.implicitHeight
                        + sshSettingsGrid.implicitHeight + 350
            } else {
                return 610
            }
        }
        Control {
            palette: approot.palette
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10

                TabBar {
                    id: connectionSettingsTabBar
                    Layout.fillWidth: true

                    palette.brightText: sysPalette.text
                    palette.dark: sysPalette.button
                    palette.mid: sysPalette.window
                    palette.window: sysPalette.base
                    palette.windowText: sysPalette.windowText

                    TabButton {
                        id: connectionWizardTabBtn
                        objectName: "rdm_connection_settings_dialog_wizard_tab"
                        text:  qsTranslate("RESP","How to connect")
                        visible: isNewConnection
                        width: visible ? undefined : 0
                    }

                    TabButton {
                        objectName: "rdm_connection_settings_dialog_basic_settings_tab"
                        text: qsTranslate("RESP","Connection Settings")
                    }

                    TabButton {
                        objectName: "rdm_connection_settings_dialog_advanced_settings_tab"
                        text:  qsTranslate("RESP","Advanced Settings")
                    }
                }

                StackLayout {
                    id: settingsTabs
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: connectionSettingsTabBar.currentIndex

                    BetterTab {
                        id: wizardTab

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: 30

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 5
                            spacing: 20

                            SettingsGroupTitle {
                                Layout.fillWidth: true
                                text: qsTranslate("RESP","Create connection from Redis URL")
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                BetterTextField {
                                    id: connectionStringField
                                    objectName: "rdm_connection_settings_dialog_wizard_connection_string_field"
                                    Layout.fillWidth: true
                                    focus: connectionWizardTabBtn.visible
                                    placeholderText: "redis://localhost:6379"

                                    KeyNavigation.tab: importConnectionStringBtn.enabled? importConnectionStringBtn : skipToNextTabBtn

                                    onAccepted: {
                                        importConnectionStringBtn.clicked()
                                    }

                                    onTextEdited: validationError = false
                                }

                                BetterButton {
                                    id: importConnectionStringBtn
                                    objectName: "rdm_connection_settings_dialog_wizard_import_btn"
                                    text: qsTranslate("RESP","Import")
                                    enabled: !!connectionStringField.text

                                    KeyNavigation.tab: skipToNextTabBtn

                                    onClicked: {
                                        if (!isConnectionStringValid(connectionStringField.text)) {
                                            connectionStringField.validationError = true
                                        } else {
                                            connectionStringField.validationError = false
                                            root.settings = parseConnectionString(connectionStringField.text)
                                            connectionSettingsTabBar.currentIndex = 1
                                            connectionName.forceActiveFocus()
                                        }
                                    }

                                    Keys.onEnterPressed: {
                                        importConnectionStringBtn.clicked()
                                    }

                                    Keys.onReturnPressed: {
                                        importConnectionStringBtn.clicked()
                                    }
                                }
                            }

                            RichTextWithLinks {
                                Layout.fillWidth: true
                                text: qsTranslate("RESP", "Learn more about Redis URL:  ")
                                      + "<a href='https://www.iana.org/assignments/uri-schemes/prov/redis'>redis://</a>,&nbsp;"
                                      + "<a href='https://www.iana.org/assignments/uri-schemes/prov/rediss'>rediss://</a>"
                            }

                            SettingsGroupTitle {
                                Layout.fillWidth: true
                                text: qsTranslate("RESP","Connection guides")
                            }

                            GridLayout {
                                id: tileGrid
                                columns: 4

                                Layout.fillWidth: true

                                property int tileSize: 90
                                property int tileIconSize: 64

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#connect-to-a-local-or-public-redis-server"

                                    tooltip: url

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    text: qsTranslate("RESP", "Local or Public Redis")

                                    showBorder: true

                                    iconSource: ""
                                    onClicked: Qt.openUrlExternally(url)
                                }

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#connect-to-a-public-redis-server-with-ssl"

                                    tooltip: url

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    text: qsTranslate("RESP", "Redis with SSL/TLS")

                                    showBorder: true

                                    iconSource: ""
                                    onClicked: Qt.openUrlExternally(url)
                                }

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#connect-to-private-redis-server-via-ssh-tunnel"

                                    tooltip: url

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    text: qsTranslate("RESP", "SSH tunnel")

                                    showBorder: true

                                    iconSource: ""
                                    onClicked: Qt.openUrlExternally(url)
                                }

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#connect-to-a-unix-socket"

                                    tooltip: url

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    text: qsTranslate("RESP", "UNIX socket")

                                    showBorder: true

                                    iconSource: ""
                                    onClicked: Qt.openUrlExternally(url)
                                }

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#digital-ocean-managed-redis"

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    imgWidth: tileGrid.tileIconSize
                                    imgHeight: tileGrid.tileIconSize

                                    iconSource: "qrc:/images/digitalocean_logo.svg"

                                    showBorder: true

                                    tooltip: url

                                    onClicked: Qt.openUrlExternally(url)
                                }

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#microsoft-azure-redis-cache"

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    imgWidth: tileGrid.tileIconSize
                                    imgHeight: tileGrid.tileIconSize

                                    iconSource: "qrc:/images/azure_logo.svg"

                                    showBorder: true

                                    tooltip: url

                                    onClicked: Qt.openUrlExternally(url)
                                }

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#aws-elasticache"

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    imgWidth: tileGrid.tileIconSize
                                    imgHeight: tileGrid.tileIconSize

                                    iconSource: approot.darkModeEnabled? "qrc:/images/aws_logo_white.svg" : "qrc:/images/aws_logo.svg"

                                    showBorder: true

                                    tooltip: url

                                    onClicked: Qt.openUrlExternally(url)
                                }

                                ImageButton {
                                    property string url: "http://docs.resp.app/en/latest/quick-start/#heroku-redis"

                                    Layout.fillWidth: true
                                    implicitHeight: tileGrid.tileSize

                                    imgWidth: tileGrid.tileIconSize
                                    imgHeight: tileGrid.tileIconSize

                                    iconSource: "qrc:/images/heroku_logo.svg"

                                    showBorder: true

                                    tooltip: url

                                    onClicked: Qt.openUrlExternally(url)
                                }                               
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.topMargin: 10

                                SettingsGroupTitle {
                                    text: qsTranslate("RESP",'Cannot figure out how to connect to your redis-server?')
                                }

                                RichTextWithLinks {
                                    Layout.fillWidth: true
                                    wrapMode: Text.WrapAnywhere
                                    html: qsTranslate("RESP",'<a href="https://docs.resp.app/en/latest/quick-start/">Read the Docs</a>, '
                                                      + '<a href="mailto:support@resp.app">Contact Support</a> '
                                                      + 'or ask for help in our <a href="https://t.me/RedisDesktopManager">Telegram Group</a>')
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.topMargin: 10

                                SettingsGroupTitle {
                                    text: qsTranslate("RESP","Don't have running Redis?")
                                }

                                RichTextWithLinks {
                                    Layout.fillWidth: true
                                    wrapMode: Text.WrapAnywhere
                                    html: '<a href="https://do.co/3humIhx">' + qsTranslate("RESP",'Spin up hassle-free Redis on Digital Ocean') + '</a>'
                                }
                            }

                            RowLayout {

                                Item { Layout.fillWidth: true }

                                Item { Layout.fillHeight: true }

                                BetterButton {
                                    id: skipToNextTabBtn
                                    text: qsTranslate("RESP","Skip")
                                    onClicked: {
                                        connectionSettingsTabBar.currentIndex = 1
                                    }

                                    Keys.onEnterPressed: {
                                        skipToNextTabBtn.clicked()
                                    }

                                    Keys.onReturnPressed: {
                                        skipToNextTabBtn.clicked()
                                    }
                                }
                            }
                        }
                    }

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

                                BetterLabel { text: qsTranslate("RESP","Name:") }

                                BetterTextField {
                                    id: connectionName
                                    objectName: "rdm_connection_name_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RESP","Connection Name")
                                    text: root.settings ? root.settings.name : ""
                                    Component.onCompleted: root.items.push(connectionName)
                                    onTextChanged: root.settings.name = text
                                }

                                BetterLabel { text: qsTranslate("RESP","Address:") }

                                AddressInput {
                                    id: connectionAddress
                                    objectName: "rdm_connection_address_input"
                                    placeholderText: qsTranslate("RESP","redis-server host")
                                    host: root.settings ? root.settings.host : ""
                                    port: root.settings ? root.settings.port : 0
                                    Component.onCompleted: root.items.push(connectionAddress)
                                    onHostChanged: if (root.settings) root.settings.host = host
                                    onPortChanged: if (root.settings) root.settings.port = port
                                }

                                BetterLabel {
                                    id: windowsLocalhostWarning
                                    Layout.columnSpan: 2
                                    text: qsTranslate("RESP", "For better network performance please use 127.0.0.1")
                                    visible: !root.sshEnabled && !root.sslEnabled
                                             && String(connectionAddress.host).toLowerCase() === "localhost"
                                             && Qt.platform.os == "windows"

                                }

                                BetterLabel { text: qsTranslate("RESP","Password:") }

                                PasswordInput {
                                    id: connectionAuth
                                    objectName: "rdm_connection_auth_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RESP","(Optional) redis-server authentication password")
                                    text: root.settings ? root.settings.auth : ""
                                    onTextChanged: root.settings.auth = text
                                }

                                BetterLabel { text: qsTranslate("RESP","Username:") }

                                BetterTextField {
                                    id: connectionUsername
                                    objectName: "rdm_connection_username_field"
                                    Layout.fillWidth: true
                                    placeholderText: qsTranslate("RESP","(Optional) redis-server authentication username" + " (Redis >6.0)")
                                    text: root.settings ? root.settings.username : ""
                                    onTextChanged: if (root.settings) root.settings.username = text
                                }
                            }

                            Item { Layout.preferredWidth: 10 }

                            SettingsGroupTitle { text: qsTranslate("RESP","Security") }

                            GridLayout {
                                id: securityGrid
                                objectName: "rdm_connection_group_box_security"
                                columns: 2

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.columnSpan: 2

                                    BetterRadioButton {
                                        id: sslRadioButton
                                        objectName: "rdm_connection_security_ssl_radio_button"
                                        text: qsTranslate("RESP","SSL / TLS")
                                        allowUncheck: true
                                        checked: root.settings ? root.settings.sslEnabled && !root.sshEnabled : false
                                        Component.onCompleted: root.sslEnabled = Qt.binding(function() { return sslRadioButton.checked })
                                        onCheckedChanged: {
                                            root.settings.sslEnabled = checked
                                            root.cleanStyle()
                                        }
                                    }

                                    BetterRadioButton {
                                        id: sshRadioButton
                                        objectName: "rdm_connection_security_ssh_radio_button"
                                        text: qsTranslate("RESP","SSH Tunnel")
                                        allowUncheck: true
                                        checked: root.settings ? root.settings.useSshTunnel() : false
                                        Component.onCompleted: root.sshEnabled = Qt.binding(function() { return sshRadioButton.checked })
                                        onCheckedChanged: {
                                            root.cleanStyle()
                                        }
                                    }
                                }

                                Item { Layout.preferredWidth: 15 }

                                GridLayout {
                                    id: tlsSettingsGrid
                                    objectName: "rdm_connection_security_ssl_grid"
                                    enabled: sslRadioButton.checked
                                    visible: sslRadioButton.checked
                                    columns: 2
                                    Layout.fillWidth: true

                                    BetterLabel { text: qsTranslate("RESP","Public Key:") }

                                    FilePathInput {
                                        id: sslLocalCertPath
                                        objectName: "rdm_connection_security_ssl_local_cert_path_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RESP","(Optional) Public Key in PEM format")
                                        nameFilters: [ "Public Key in PEM format (*.pem *.crt)" ]
                                        title: qsTranslate("RESP","Select public key in PEM format")
                                        path: root.settings ? root.settings.sslLocalCertPath : ""
                                        onPathChanged: root.settings.sslLocalCertPath = path
                                    }

                                    BetterLabel { text: qsTranslate("RESP", "Private Key") + ":" }

                                    FilePathInput {
                                        id: sslPrivateKeyPath
                                        objectName: "rdm_connection_security_ssl_private_key_path_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RESP","(Optional) Private Key in PEM format")
                                        nameFilters: [ "Private Key in PEM format (*.pem *.key)" ]
                                        title: qsTranslate("RESP","Select private key in PEM format")
                                        path: root.settings ? root.settings.sslPrivateKeyPath : ""
                                        onPathChanged: root.settings.sslPrivateKeyPath = path
                                    }

                                    BetterLabel { text: qsTranslate("RESP","Authority:") }

                                    FilePathInput {
                                        id: sslCaCertPath
                                        objectName: "rdm_connection_security_ssl_ca_cert_path_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RESP","(Optional) Authority in PEM format")
                                        nameFilters: [ "Authority file in PEM format (*.pem *.crt)" ]
                                        title: qsTranslate("RESP","Select authority file in PEM format")
                                        path: root.settings ? root.settings.sslCaCertPath : ""
                                        onPathChanged: root.settings.sslCaCertPath = path
                                    }

                                    BetterLabel { text: qsTranslate("RESP","Enable strict mode:")}

                                    BetterCheckbox {
                                        id: ignoreSSLErrors
                                        Layout.fillWidth: true
                                        checked: root.settings ? !root.settings.ignoreSSLErrors : false
                                        onCheckedChanged: root.settings.ignoreSSLErrors = !checked
                                    }
                                }

                                GridLayout {
                                    id: sshSettingsGrid
                                    objectName: "rdm_connection_security_ssh_grid"
                                    visible: sshRadioButton.checked
                                    enabled: sshRadioButton.checked
                                    columns: 2
                                    Layout.fillWidth: true

                                    BetterLabel { text: qsTranslate("RESP","SSH Address:") }

                                    AddressInput {
                                        id: sshAddress
                                        placeholderText: qsTranslate("RESP","Remote Host with SSH server")
                                        port: root.settings ? root.settings.sshPort : 22
                                        host: root.settings ? root.settings.sshHost : ""
                                        Component.onCompleted: root.sshItems.push(sshAddress)
                                        onHostChanged: root.settings.sshHost = host
                                        onPortChanged: root.settings.sshPort = port
                                    }

                                    BetterLabel { text: qsTranslate("RESP","SSH User:") }

                                    BetterTextField {
                                        id: sshUser
                                        objectName: "rdm_connection_security_ssh_user_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RESP","Valid SSH User Name")
                                        text: root.settings ? root.settings.sshUser : ""
                                        Component.onCompleted: root.sshItems.push(sshUser)
                                        onTextChanged: root.settings.sshUser = text
                                    }

                                    BetterCheckbox {
                                        id: sshAgentCheckbox
                                        objectName: "rdm_connection_security_ssh_agent"
                                        text: qsTranslate("RESP","Use SSH Agent")
                                        checked: root.settings ? root.settings.sshAgent : false
                                        onCheckedChanged: root.settings.sshAgent = checked
                                    }

                                    FilePathInput {
                                        id: sshAgentPath
                                        visible: !(Qt.platform.os === "windows" || (PlatformUtils.isOSX() && qmlUtils.isAppStoreBuild()))
                                        objectName: "rdm_connection_security_ssh_agent_path_field"
                                        Layout.fillWidth: true
                                        placeholderText: qsTranslate("RESP","(Optional) Custom SSH Agent Path")
                                        nameFilters: [ "SSH Agent (*)" ]
                                        title: qsTranslate("RESP","Select SSH Agent")
                                        path: root.settings ? root.settings.sshAgentPath : ""
                                        onPathChanged: root.settings.sshAgentPath = path
                                    }

                                    RichTextWithLinks {
                                        visible: PlatformUtils.isOSX() && qmlUtils.isAppStoreBuild()
                                        Layout.fillWidth: true
                                        wrapMode: Text.WrapAnywhere
                                        html: '<a href="https://docs.resp.app/en/latest/quick-start/#ssh-agent">' + qsTranslate("RESP",'Additional configuration is required to enable SSH Agent support') + '</a>'
                                    }

                                    BetterGroupbox {
                                        id: sshKeyGroupBox
                                        labelText: qsTranslate("RESP","Private Key")
                                        objectName: "rdm_connection_security_ssh_key_group_box"
                                        checked: root.settings ? root.settings.sshPrivateKey : false                                        
                                        enabled: !sshAgentCheckbox.checked
                                        opacity: enabled ? 1 : 0.5

                                        Layout.columnSpan: 2
                                        Layout.fillWidth: true

                                        ColumnLayout {
                                            anchors.fill: parent

                                            FilePathInput {
                                                id: sshPrivateKey
                                                objectName: "rdm_connection_security_ssh_key_path_field"

                                                Layout.fillWidth: true

                                                placeholderText: qsTranslate("RESP","Path to Private Key in PEM format")
                                                nameFilters: [ "Private key in PEM format (*)" ]
                                                title: qsTranslate("RESP","Select private key in PEM format")
                                                path: root.settings ? root.settings.sshPrivateKey : ""
                                                onPathChanged: root.settings.sshPrivateKey = path
                                            }

                                            BetterLabel {
                                                visible: PlatformUtils.isOSX()
                                                Layout.fillWidth: true;
                                                text: qsTranslate("RESP","<b>Tip:</b> Use <code>⌘ + Shift + .</code> to show hidden files and folders in dialog") }
                                        }
                                    }

                                    BetterGroupbox {
                                        id: sshPasswordGroupBox
                                        labelText: sshKeyGroupBox.checked? qsTranslate("RESP","Passphrase") : qsTranslate("RESP","Password")
                                        objectName: "rdm_connection_security_ssh_password_group_box"
                                        checked: root.settings ? root.settings.sshPassword || root.settings.askForSshPassword : true
                                        enabled: !sshAgentCheckbox.checked
                                        opacity: enabled ? 1 : 0.5

                                        Layout.columnSpan: 2
                                        Layout.fillWidth: true

                                        RowLayout {
                                            anchors.fill: parent

                                            PasswordInput {
                                                id: sshPassword
                                                objectName: "rdm_connection_security_ssh_password_field"

                                                Layout.fillWidth: true
                                                placeholderText: sshKeyGroupBox.checked? qsTranslate("RESP","Passphrase for provided private key")
                                                                                       : sshAskForPasswordCheckbox.checked?
                                                                                             qsTranslate("RESP","Password request will be prompt prior to connection")
                                                                                           : qsTranslate("RESP","SSH User Password")
                                                text: root.settings ? root.settings.sshPassword : ""
                                                onTextChanged: root.settings.sshPassword = text

                                                enabled: !sshAskForPasswordCheckbox.checked
                                            }

                                            BetterCheckbox {
                                                id: sshAskForPasswordCheckbox
                                                objectName: "rdm_connection_security_ssh_ask_for_password"
                                                text: qsTranslate("RESP","Ask for password")
                                                checked: root.settings ? root.settings.askForSshPassword : false
                                                onCheckedChanged: root.settings.askForSshPassword = checked
                                            }
                                        }
                                    }

                                    BetterCheckbox {
                                        id: sshTLSoverSSHCheckbox
                                        objectName: "rdm_connection_security_ssh_tls_over_ssh"
                                        Layout.fillWidth: true
                                        Layout.columnSpan: 2
                                        text: qsTranslate("RESP","Enable TLS-over-SSH (<b>AWS ElastiCache</b> <b>Encryption in-transit</b>)")
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
                                text: qsTranslate("RESP","Keys loading")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RESP","Default filter:") }

                            BetterTextField
                            {
                                id: keysPattern
                                Layout.fillWidth: true
                                placeholderText: qsTranslate("RESP","Pattern which defines loaded keys from redis-server")
                                text: root.settings ? root.settings.keysPattern : ""
                                Component.onCompleted: root.items.push(keysPattern)
                                onTextChanged: if (root.settings) { root.settings.keysPattern = text }
                            }

                            BetterLabel { text: qsTranslate("RESP","Namespace Separator:") }

                            BetterTextField
                            {
                                id: namespaceSeparator
                                Layout.fillWidth: true
                                objectName: "rdm_advanced_settings_namespace_separator_field"
                                placeholderText: qsTranslate("RESP","Separator used for namespace extraction from keys")
                                text: root.settings ? root.settings.namespaceSeparator : ""
                                onTextChanged: if (root.settings) { root.settings.namespaceSeparator = text }
                            }

                            SettingsGroupTitle {
                                text: qsTranslate("RESP","Timeouts & Limits")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RESP","Connection Timeout (sec):") }

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

                            BetterLabel { text: qsTranslate("RESP","Execution Timeout (sec):")}

                            BetterSpinBox {
                                id: connectionTimeout
                                Layout.fillWidth: true
                                from: 10
                                to: 100000
                                value: root.settings ? (root.settings.connectionTimeout / 1000.0) : 0
                                onValueChanged: if (root.settings) { root.settings.connectionTimeout = value * 1000 }
                            }

                            BetterLabel { text: qsTranslate("RESP","Databases discovery limit:") }

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
                                text: qsTranslate("RESP","Cluster")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RESP","Change host on cluster redirects:")}

                            BetterCheckbox {
                                id: overrideClusterHost
                                Layout.fillWidth: true
                                checked: root.settings ? root.settings.overrideClusterHost : false
                                onCheckedChanged: if (root.settings) { root.settings.overrideClusterHost = checked }
                            }

                            SettingsGroupTitle {
                                text: qsTranslate("RESP","Formatters")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RESP","Default value formatter:")}

                            RowLayout {
                                Layout.fillWidth: true

                                BetterComboBox {
                                    id: defaultFormatterLogicSelector

                                    Layout.fillWidth: true

                                    property int customFormatterIndex: 2

                                    ListModel {
                                        id: defaultFormatterOptionsModel

                                        Component.onCompleted: {
                                            append({ value: "auto", text: qsTranslate("RESP", "Auto detect (JSON / Plain Text / HEX)") })
                                            append({ value: "last_selected", text: qsTranslate("RESP", "Last selected") })
                                            append({ value: "specific", text: qsTranslate("RESP", "Select formatter ...") })
                                            defaultFormatterLogicSelector.currentIndex = 0
                                        }
                                    }

                                    textRole: "text"
                                    model: defaultFormatterOptionsModel

                                    Connections {
                                        target: root

                                        function onSettingsChanged(s) {
                                            if (!root.settings) {
                                                defaultFormatterLogicSelector.currentIndex = 0;
                                                return;
                                            }

                                            if (root.settings.defaultFormatter !== "auto"
                                                    && root.settings.defaultFormatter !== "last_selected") {
                                                defaultFormatterSelector._select(root.settings.defaultFormatter)
                                                defaultFormatterLogicSelector.currentIndex = defaultFormatterLogicSelector.customFormatterIndex;
                                                return;
                                            }

                                            defaultFormatterLogicSelector.currentIndex = root.settings.defaultFormatter === "auto"? 0 : 1;
                                        }
                                    }

                                    onActivated: {
                                        if (currentIndex != customFormatterIndex) {
                                            root.settings.defaultFormatter = defaultFormatterOptionsModel.get(currentIndex)["value"]
                                        }
                                    }
                                }

                                BetterComboBox {
                                    id: defaultFormatterSelector

                                    visible: defaultFormatterLogicSelector.currentIndex == 2
                                    model: valueFormattersModel
                                    textRole: "name"

                                    onActivated: {
                                        root.settings.defaultFormatter = currentText
                                    }
                                }
                            }

                            SettingsGroupTitle {
                                text: qsTranslate("RESP","Appearance")
                                Layout.columnSpan: 2
                            }

                            BetterLabel { text: qsTranslate("RESP","Icon color:")}

                            ColorInput {
                                id: iconsColor
                                Layout.fillWidth: true

                                color: root.settings ? root.settings.iconColor : ""
                                onColorChanged: root.settings.iconColor = color

                                Connections {
                                    target: root

                                    function onResetSettings() {
                                        iconsColor.reset();
                                    }
                                }
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

                    visible: !isNewConnection || isNewConnection && connectionSettingsTabBar.currentIndex != 0

                    BetterButton {
                        objectName: "rdm_connection_settings_dialog_test_btn"
                        iconSource: PlatformUtils.getThemeIcon("offline.svg")
                        text: qsTranslate("RESP","Test Connection")
                        onClicked: {
                            showLoader()
                            root.testConnection(root.settings)
                        }
                    }

                    BetterButton {
                        iconSource: PlatformUtils.getThemeIcon("help.svg")
                        text: qsTranslate("RESP","Quick Start Guide")
                        onClicked: Qt.openUrlExternally(root.quickStartGuideUrl)
                        visible: !isNewConnection
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
                            source: PlatformUtils.getThemeIcon("alert.svg")
                        }
                        BetterLabel {
                            text: qsTranslate("RESP","Invalid settings detected!")
                        }
                    }

                    Item { Layout.fillWidth: true }

                    BetterButton {
                        objectName: "rdm_connection_settings_dialog_ok_btn"
                        text: qsTranslate("RESP","OK")
                        onClicked: {
                            if (root.validate()) {

                                if (!sshKeyGroupBox.checked)
                                    root.settings.sshPrivateKey = ""
                                if (!sshPasswordGroupBox.checked)
                                    root.settings.sshPassword = ""

                                if (sshAgentCheckbox.checked) {
                                    root.settings.sshPrivateKey = ""
                                    root.settings.sshPassword = ""
                                } else {
                                    root.settings.sshAgentPath = ""
                                }

                                root.saveConnection(root.settings)
                                root.settings = connectionsManager.createEmptyConfig()
                                root.resetSettings()
                                root.close()
                            } else {
                                validationWarning.visible = true
                            }
                        }
                    }

                    BetterButton {
                        objectName: "rdm_connection_settings_dialog_cancel_btn"
                        text: qsTranslate("RESP","Cancel")
                        onClicked: {
                            root.settings = connectionsManager.createEmptyConfig()
                            root.cleanStyle()
                            root.resetSettings()
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
            OkDialogOverlay {
                id: dialog_notification

                objectName: "rdm_qml_connection_settings_error_dialog"
                visible: false                                              

                function showError(msg) {                    
                    text = msg
                    title = qsTranslate("RESP","Error")
                    open()
                }

                function showMsg(msg) {                    
                    text = msg
                    title = qsTranslate("RESP","Success")
                    open()
                }
            }
        }
    }
}
