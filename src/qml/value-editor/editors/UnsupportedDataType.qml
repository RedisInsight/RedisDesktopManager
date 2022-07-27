import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import "./../../common"
import "."

AbstractEditor {
    id: root
    anchors.fill: parent

    property bool active: false
    property string keyType: ""

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }

    BetterLabel {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        text: qsTranslate("RESP","Unsupported Redis Data type ") + keyType
        font.pixelSize: 16
    }

    Loader {
        id: betaSupportIsAvailable
        Layout.fillWidth: true
        asynchronous: true
        source: keyType? "https://resp.app/qml/BetaModuleSupport.qml?app_version="
                + Qt.application.version + "&platform=" + Qt.platform.os
                + "&module=" + encodeURIComponent(keyType) + "&t=" + Date.now() : ""
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }

    onKeyTypeChanged: {
        uiBlocker.visible = false;
    }

    function initEmpty() {
    }

    function isEdited() {
        return false
    }

    function reset() {
        active = false
    }
}
