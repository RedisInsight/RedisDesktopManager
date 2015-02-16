import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ColumnLayout {
    id: pagination
    visible: showValueNavigation

    Text {
        Layout.maximumWidth: 130
        text: "Page " + table.currentPage + " of " + table.totalPages
        wrapMode: Text.WrapAnywhere
    }
    RowLayout {
        Layout.maximumWidth: 130
        spacing: 0
        Button {
            Layout.maximumWidth: 65
            text: "⇦"
            onClicked: table.goToPrevPage()
        }
        Button {
            Layout.maximumWidth: 65
            text: "⇨"
            onClicked: table.goToNextPage()
        }
    }

    TextField {
        id: pageField;
        text: "1";
        Layout.maximumWidth: 130;
        Layout.preferredWidth: 130;
        readOnly: false
        validator: IntValidator {bottom: 1; top: table.totalPages}
    }

    Button {
        Layout.preferredWidth: 130
        text: "Goto Page"
        onClicked: table.goToPage(pageField.text)
    }   
}
