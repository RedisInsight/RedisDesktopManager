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
                qmlUtils.copyToClipboard(result + ":*")
            }
        },
    }

    model:
        [
            {'icon': "qrc:/images/refresh.svg", "event": "reload", "help": qsTranslate("RDM","Reload Namespace")},
            {'icon': "qrc:/images/copy.svg", "callback": "copy", "help": qsTranslate("RDM","Copy Namespace Pattern")},
            {'icon': "qrc:/images/delete.svg", "event": "delete", "help": qsTranslate("RDM","Delete Namespace")}
        ]
}

