import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import "../common"

ColumnLayout {    
    RowLayout {
        Layout.fillWidth: true

        Text {
            text: qsTranslate("RDM","Page") + " "
            wrapMode: Text.WrapAnywhere
        }

        Text {
            id: pageField;
            text: table.currentPage;
            Layout.maximumWidth: 60;
        }

        Text {
            Layout.maximumWidth: 130
            text: " of " + table.totalPages
            wrapMode: Text.WrapAnywhere
        }
    }

    RowLayout {        
        Layout.maximumWidth: 200
        Layout.fillWidth: true
        spacing: 1
        BetterButton {
            Layout.fillWidth: true
            text: "⇦"
            onClicked: table.goToPrevPage()
        }
        BetterButton {
            Layout.fillWidth: true
            text: "⇨"
            onClicked: table.goToNextPage()
            objectName: "rdm_value_editor_next_page_button"
        }
    }
}
