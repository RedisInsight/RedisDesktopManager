import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."
import "./../../common/platformutils.js" as PlatformUtils

InlineMenu {
    id: root

    callbacks: {
        "copy": function() {
            var result = styleData.value["full_name"]

            if (result) {
                qmlUtils.copyToClipboard(result)
            }
        },
    }

    model:
        [
            {'icon': PlatformUtils.getThemeIcon("copy.svg"), "callback": "copy", "help": qsTranslate("RESP","Copy Key Name"), "shortcut": "Ctrl+C"},
            {'icon': PlatformUtils.getThemeIcon("delete.svg"), "event": "delete", "help": qsTranslate("RESP","Delete key"), "shortcut": "D"}
        ]
}
