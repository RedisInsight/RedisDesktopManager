import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import "../common"

ColumnLayout {    
    GridLayout {
        columns: 2
        Layout.fillWidth: true

        BetterLabel {
            text: qsTranslate("RDM","Page") + ":"
            wrapMode: Text.WrapAnywhere
        }

        BetterTextField {
            id: pageField;

            text: table.currentPage;

            tooltip: qsTranslate("RDM", "Total pages: ") + table.totalPages

            Layout.fillWidth: true

            validator: IntValidator {
              locale: pageField.locale.name
              bottom: 1
              top: table.totalPages
            }

            onFocusChanged: {
                if (focus)
                    return;

                text = Qt.binding(function() { return table.currentPage; });
            }

            onAccepted: {
                table.goToPage(text)
            }
        }

        BetterLabel {
            Layout.columnSpan: 2
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
