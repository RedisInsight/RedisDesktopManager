import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ColumnLayout {
    id: pagination
    visible: showValueNavigation

    RowLayout {
        Text {
            text: "Page "
            wrapMode: Text.WrapAnywhere
        }

        TextField {
            id: pageField;
            text: table.currentPage;
            Layout.maximumWidth: 60;
            Layout.preferredWidth: 60;
            readOnly: false
            validator: IntValidator {bottom: 1; top: table.totalPages}
        }

        Text {
            Layout.maximumWidth: 130
            text: " of " + table.totalPages
            wrapMode: Text.WrapAnywhere
        }
    }

    RowLayout {
        Layout.maximumWidth: 100
        spacing: 0
        Button {
            text: "⇦"
            onClicked: table.goToPrevPage()
        }
        Button {
            text: "Set"
            onClicked: table.goToPage(pageField.text)
        }
        Button {
            text: "⇨"
            onClicked: table.goToNextPage()
        }
    }
}
