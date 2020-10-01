import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import "../common"

ColumnLayout {    
    GridLayout {
        columns: 3
        Layout.fillWidth: true

        BetterLabel {
            text: qsTranslate("RDM","Page") + " "
            wrapMode: Text.WrapAnywhere
        }

        BetterLabel {
            id: pageField;
            text: table.currentPage;
            Layout.maximumWidth: 60;
        }

        BetterLabel {
            Layout.maximumWidth: 130
            text: " of " + table.totalPages
            wrapMode: Text.WrapAnywhere
        }

        BetterLabel {
            Layout.columnSpan: 3
            text:  qsTranslate("RDM","Size: ") + keyRowsCount
        }
    }

    RowLayout {        
        Layout.maximumWidth: 200
        Layout.fillWidth: true
        spacing: 1
        BetterButton {
            Layout.fillWidth: true
            palette.buttonText: sysPalette.dark
            text: "❮"
            onClicked: table.goToPrevPage()
        }
        BetterButton {
            Layout.fillWidth: true
            palette.buttonText: sysPalette.dark
            text: "❯"
            onClicked: table.goToNextPage()
            objectName: "rdm_value_editor_next_page_button"
        }
    }
}
