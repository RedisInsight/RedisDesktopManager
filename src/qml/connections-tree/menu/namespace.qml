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

    model: {
        if (styleData.value["locked"] === true) {
            return [
                        {
                            'icon': "qrc:/images/offline.svg", 'event': 'cancel', "help": qsTranslate("RDM","Disconnect"),
                        },
                    ]
        } else {
            [
                {'icon': "qrc:/images/refresh.svg", "event": "reload", "help": qsTranslate("RDM","Reload Namespace")},
                {'icon': "qrc:/images/add.svg", 'event': 'add_key', "help": qsTranslate("RDM","Add New Key")},
                {'icon': "qrc:/images/copy.svg", "callback": "copy", "help": qsTranslate("RDM","Copy Namespace Pattern")},
                {'icon': "qrc:/images/memory_usage.svg", "event": "analyze_memory_usage", "help": qsTranslate("RDM","Analyze Used Memory")},
                {'icon': "qrc:/images/delete.svg", "event": "delete", "help": qsTranslate("RDM","Delete Namespace")},                
            ]
        }
    }
}

