import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."
import "./../../common/platformutils.js" as PlatformUtils

InlineMenu {
    id: root

    model: {
        return [
                    {
                        'icon': PlatformUtils.getThemeIcon("settings.svg"), 'event': 'edit', "help": qsTranslate("RESP","Edit Connection Group"),
                        "shortcut": "Ctrl+E",
                    },
                    {
                        'icon': PlatformUtils.getThemeIcon("delete.svg"), 'event': 'delete', "help": qsTranslate("RESP","Delete Connection Group"),
                        "shortcut": "D",
                    },
                ]
    }
}
