import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

InlineMenu {
    id: root

    model: {
        return [
                    {
                        'icon': "qrc:/images/settings.svg", 'event': 'edit', "help": qsTranslate("RESP","Edit Connection Group"),
                        "shortcut": Qt.platform.os == "osx"? "Meta+E" : "Ctrl+E",
                    },
                    {
                        'icon': "qrc:/images/delete.svg", 'event': 'delete', "help": qsTranslate("RESP","Delete Connection Group"),
                        "shortcut": Qt.platform.os == "osx"? "Meta+Del" : "Ctrl+Del",
                    },
                ]
    }
}
