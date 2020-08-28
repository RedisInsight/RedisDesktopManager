import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

InlineMenu {
    id: root

    model: {
        return [
                    {
                        'icon': "qrc:/images/editdb.svg", 'event': 'edit', "help": qsTranslate("RDM","Edit Connection Group"),
                        "shortcut": Qt.platform.os == "osx"? "Meta+E" : "Ctrl+E",
                    },
                    {
                        'icon': "qrc:/images/delete.svg", 'event': 'delete', "help": qsTranslate("RDM","Delete Connection Group"),
                        "shortcut": Qt.platform.os == "osx"? "Meta+Del" : "Ctrl+Del",
                    },
                ]
    }
}
