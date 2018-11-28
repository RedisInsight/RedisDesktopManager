import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

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
        spacing: 0
        Button {
            Layout.fillWidth: true
            text: "⇦"
            onClicked: table.goToPrevPage()
        }
        Button {
            Layout.fillWidth: true
            text: "⇨"
            onClicked: table.goToNextPage()
            objectName: "rdm_value_editor_next_page_button"
        }
    }
}
