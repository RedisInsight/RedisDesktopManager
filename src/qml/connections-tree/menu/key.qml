import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

RowLayout {
    id: root


    InlineMenu {
        callbacks: {
            "copy": function() {
                if (!connectionsManager)
                    return

                var result = connectionsManager.data(styleData.index, 258) // 258 - original name role

                if (result) {
                    qmlUtils.copyToClipboard(result)
                }
            },
        }

        model:
            [
                {'icon': "qrc:/images/copy.svg", "callback": "copy", "help": "Copy Key Name"},
                {'icon': "qrc:/images/delete.svg", "event": "delete", "help": "Delete Key"}
            ]
    }

}
