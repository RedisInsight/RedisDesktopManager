import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

RowLayout {
    id: pagination
    visible: showValueNavigation

    Button {
        text: "⇤"
        onClicked: table.goToFirstPage()
    }
    Button {
        text: "⇦"
        onClicked: table.goToPrevPage()
    }
    Text {
        text: "Page " + table.currentPage + " of " + table.totalPages
                + " (Items:" + (table.currentStart+1) + "-"
                + (table.currentStart+table.rowCount)
                + " of " + table.model.totalRowCount() + ")"
    }
    TextField { text: "1"; Layout.fillWidth: true; readOnly: false}
    Button {
        text: "Goto Page"
        onClicked: {}
    }
    Button {
        text: "⇨"
        onClicked: table.goToNextPage()
    }
    Button {
        text: "⇥"
        onClicked: table.goToLastPage()
    }
}
