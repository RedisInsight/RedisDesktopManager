import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

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
            {'icon': "qrc:/images/copy.svg", "callback": "copy", "help": qsTranslate("RDM","Copy Key Name")},
            {'icon': "qrc:/images/delete.svg", "event": "delete", "help": qsTranslate("RDM","Delete key")}
        ]
}
