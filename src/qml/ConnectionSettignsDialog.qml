import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import "./common"

Dialog {
    id: root
    title: !settings || !settings.name ? "New Connection Settings" : "Edit Connection Settings - " + settings.name

    property var settings
    property string quickStartGuideUrl: "https://github.com/uglide/RedisDesktopManager/wiki/Quick-Start"

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
        implicitHeight: 675

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 5

            TabView {
                id: settingsTabs
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 590

                Tab {
                    id: mainTab
                    title: "Connection Settings"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10

                        GroupBox {
                            title: "Main Settings"
                            Layout.fillWidth: true

                            GridLayout {
                                anchors.fill: parent
                                columns: 2

                                Label { text: "Name:" }

                                TextField {
                                    id: connectionName
                                    Layout.fillWidth: true
                                    placeholderText: "Connection Name"
                                    text: root.settings ? root.settings.name : ""                                    
                                    Component.onCompleted: root.items.push(connectionName)
                                    onTextChanged: root.settings.name = text
                                }

                                Label { text: "Address:" }

                                AddressInput {
                                    id: connectionAddress
                                    placeholderText: "redis-server host"
                                    host: root.settings ? root.settings.host : "127.0.0.1"
                                    port: root.settings ? root.settings.port : 6379
                                    Component.onCompleted: root.items.push(connectionAddress)
                                    onHostChanged: if (root.settings) root.settings.host = host
                                    onPortChanged: if (root.settings) root.settings.port = port
                                }

                                Label { text: "Auth:" }

                                PasswordInput {
                                    id: connectionAuth
                                    Layout.fillWidth: true
                                    placeholderText: "(Optional) redis-server authentication password"
                                    text: root.settings ? root.settings.auth : ""
                                    onTextChanged: root.settings.auth = text
                                }
                            }
                        }

                        GroupBox {
                            title: "Security"

                            Layout.columnSpan: 2
                            Layout.fillWidth: true

                            ExclusiveGroup { id: connectionSecurityExGroup }

                            GridLayout {
                                anchors.fill: parent
                                columns: 2

                                RadioButton {
                                    text: "None"
                                    checked: root.settings ? !root.settings.useSsl() && !root.settings.useSshTunnel() : true
                                    exclusiveGroup: connectionSecurityExGroup
                                    Layout.columnSpan: 2
                                }

                                RadioButton {
                                    id: sslRadioButton
                                    Layout.columnSpan: 2
                                    text: "SSL"
                                    exclusiveGroup: connectionSecurityExGroup
                                    checked: root.settings ? root.settings.useSsl() : false
                                    Component.onCompleted: root.sslEnabled = Qt.binding(function() { return sslRadioButton.checked })
                                    onCheckedChanged: {
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

                                    Label { text: "Public Key:" }

                                    FilePathInput {
                                        id: sslLocalCertPath
                                        Layout.fillWidth: true
                                        placeholderText: "Public Key in PEM format"
                                        nameFilters: [ "Public Key in PEM format (*)" ]
                                        title: "Select public key in PEM format"
                                        path: root.settings ? root.settings.sslLocalCertPath : ""
                                        Component.onCompleted: root.sslItems.push(sslLocalCertPath)
                                        onPathChanged: root.settings.sslLocalCertPath = path
                                    }

                                    Label { text: "Private Key:" }

                                    FilePathInput {
                                        id: sslPrivateKeyPath
                                        Layout.fillWidth: true
                                        placeholderText: "(Optional) Private Key in PEM format"
                                        nameFilters: [ "Private Key in PEM format (*)" ]
                                        title: "Select private key in PEM format"
                                        path: root.settings ? root.settings.sslPrivateKeyPath : ""
                                        onPathChanged: root.settings.sslPrivateKeyPath = path
                                    }

                                    Label { text: "Authority:" }

                                    FilePathInput {
                                        id: sslCaCertPath
                                        Layout.fillWidth: true
                                        placeholderText: "(Optional) Authority in PEM format"
                                        nameFilters: [ "Authority file in PEM format (*)" ]
                                        title: "Select authority file in PEM format"
                                        path: root.settings ? root.settings.sslCaCertPath : ""
                                        onPathChanged: root.settings.sslCaCertPath = path
                                    }
                                }

                                RadioButton {
                                    id: sshRadioButton
                                    Layout.columnSpan: 2
                                    text: "SSH Tunnel"
                                    exclusiveGroup: connectionSecurityExGroup
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

                                    Label { text: "SSH Address:" }

                                    AddressInput {
                                        id: sshAddress
                                        placeholderText: "Remote Host with SSH server"
                                        port: root.settings ? root.settings.sshPort : 22
                                        host: root.settings ? root.settings.sshHost : ""
                                        Component.onCompleted: root.sshItems.push(sshAddress)
                                        onHostChanged: root.settings.sshHost = host
                                        onPortChanged: root.settings.sshPort = port
                                    }

                                    Label { text: "SSH User:" }

                                    TextField {
                                        id: sshUser
                                        Layout.fillWidth: true
                                        placeholderText: "Valid SSH User Name"
                                        text: root.settings ? root.settings.sshUser : ""
                                        Component.onCompleted: root.sshItems.push(sshUser)
                                        onTextChanged: root.settings.sshUser = text
                                    }

                                    GroupBox {
                                        title: "Private Key"
                                        checkable: true
                                        checked: root.settings ? root.settings.sshPrivateKey : false

                                        Layout.columnSpan: 2
                                        Layout.fillWidth: true

                                        FilePathInput {
                                            id: sshPrivateKey
                                            anchors.fill: parent
                                            placeholderText: "Path to Private Key in PEM format"
                                            nameFilters: [ "Private key in PEM format (*)" ]
                                            title: "Select private key in PEM format"
                                            path: root.settings ? root.settings.sshPrivateKey : ""
                                            onPathChanged: root.settings.sshPrivateKey = path
                                        }
                                    }

                                    GroupBox {
                                        title: "Password"
                                        checkable: true
                                        checked: root.settings ? root.settings.sshPassword : true

                                        Layout.columnSpan: 2
                                        Layout.fillWidth: true

                                        PasswordInput {
                                            id: sshPassword
                                            anchors.fill: parent
                                            placeholderText: "SSH User Password"
                                            text: root.settings ? root.settings.sshPassword : ""
                                            onTextChanged: root.settings.sshPassword = text
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Tab {
                    title: "Advanced Settings"

                    GridLayout {
                        anchors.fill: parent
                        anchors.margins: 10

                        columns: 2

                        Label { text: "Keys glob-style pattern:" }

                        TextField
                        {
                            id: keysPattern
                            Layout.fillWidth: true
                            placeholderText: "Pattern which defines loaded keys from redis-server"
                            text: root.settings ? root.settings.keysPattern : "*"
                            Component.onCompleted: root.items.push(keysPattern)
                            onTextChanged: root.settings.keysPattern = text
                        }

                        Label { text: "Namespace Separator:" }

                        TextField
                        {
                            id: namespaceSeparator
                            Layout.fillWidth: true
                            placeholderText: "Separator used for namespace extraction from keys"
                            text: root.settings ? root.settings.namespaceSeparator : ":"
                            onTextChanged: root.settings.namespaceSeparator = text
                        }

                        Label { text: "Connection Timeout (sec):"}

                        SpinBox {
                            id: executeTimeout
                            Layout.fillWidth: true
                            minimumValue: 30
                            maximumValue: 100000
                            value: {
                                console.log("Execution timeout:", root.settings.executeTimeout)
                                return root.settings ? (root.settings.executeTimeout / 1000.0) : 60
                            }
                            onValueChanged: root.settings.executeTimeout = value * 1000
                        }

                        Label { text: "Execution Timeout (sec):"}

                        SpinBox {
                            id: connectionTimeout
                            Layout.fillWidth: true
                            minimumValue: 30
                            maximumValue: 100000
                            value: root.settings ? (root.settings.connectionTimeout / 1000.0) : 60
                            onValueChanged: root.settings.connectionTimeout = value * 1000
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
                    Text { text: "Invalid settings detected!"}
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    iconSource: "qrc:/images/offline.svg"
                    text: "Test Connection"
                    onClicked: root.testConnection(root.settings)
                }

                ToolButton {
                    iconSource: "qrc:/images/help.svg"
                    text: "Quick Start Guide"
                    tooltip: text
                    onClicked: Qt.openUrlExternally(root.quickStartGuideUrl)
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: "OK"
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
                    text: "Cancel"
                    onClicked: root.close()
                }
            }
        }
    }
}
