import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtWebEngine 1.1

ColumnLayout {
    id: root

    property bool debugConsole: false

    RowLayout {
        ToolButton {
            iconSource: "qrc:/images/execute.png"
        }
        ToolButton {
            iconSource: "qrc:/images/debug.png"
        }
        ToolButton {
            iconSource: "qrc:/images/next-line.png"
        }
        ToolButton {
            iconSource: "qrc:/images/stop.png"
        }
    }

    WebEngineView {
        id: webview
        Layout.fillWidth: true
        Layout.fillHeight: true

        url: "qrc:/static/ace.html"

        onJavaScriptConsoleMessage: {
            log.append(level + ":" + message + ":" + lineNumber + "(" + sourceID + ")")
        }
    }

    TextArea {
        visible: root.debugConsole
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        id: log
    }
}


