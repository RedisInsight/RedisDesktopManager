import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

RowLayout {
    InlineMenu {
        model: [
            {'icon': "qrc:/images/console.png", 'event': 'console'},
            {'icon': "qrc:/images/refresh.png", 'event': 'reload'},
            {'icon': "qrc:/images/offline.png", 'event': 'unload'},
            {'icon': "qrc:/images/editdb.png", 'event': 'edit'},
            {'icon': "qrc:/images/delete.png", 'event': 'delete'},
        ]
    }
}
