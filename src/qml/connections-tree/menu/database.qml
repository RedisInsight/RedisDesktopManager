import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "."

RowLayout {
    InlineMenu {
        model: [
            {'icon': "qrc:/images/refresh.png", 'event': 'reload'},
            {'icon': "qrc:/images/add.png", 'event': 'add_key'},
            {'icon': "qrc:/images/filter.png", 'event': 'set_filter'},
            {'icon': "qrc:/images/clear.png", 'event': 'reset_filter'},
        ]
    }
}
