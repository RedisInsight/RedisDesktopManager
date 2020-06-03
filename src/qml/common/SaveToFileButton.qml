import QtQuick 2.0
import QtQuick.Dialogs 1.2

ImageButton {
    iconSource: "qrc:/images/save_as.png"
    tooltip: qsTranslate("RDM","Save to File")

    onClicked: {
        saveValueToFileDialog.open()
    }

    FileDialog {
        id: saveValueToFileDialog
        title: qsTranslate("RDM","Save Value")
        nameFilters: ["All files (*)"]
        selectExisting: false
        onAccepted: {
            var path = qmlUtils.getPathFromUrl(fileUrl)
            if (qmlUtils.saveToFile(value, path)) {
                saveToFileConfirmation.text = qsTranslate("RDM","Value will be saved to file: ") + "\n" + path
                saveToFileConfirmation.open()
            }
        }
    }

    OkDialog {
        id: saveToFileConfirmation
        title: qsTranslate("RDM","Save value to file")
        text: ""
        visible: false
    }
}
