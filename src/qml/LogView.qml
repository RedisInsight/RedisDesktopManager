import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import "./common"

FastTextView {
    id: root

    property alias eventsModel: modelConnections.target

    model: ListModel {}

    function dumpText() {
        var allStrings = "";
        for (var ind=0; ind < root.model.count; ind++) {
            allStrings += root.model.get(ind)["msg"] + "\n"
        }
        return allStrings
    }

    color: sysPalette.base
    border.color: sysPalette.shadow
    border.width: 1
    showLineNumbers: false

    Connections {
        id: modelConnections
        function onLog(msg) {
            if (model.count > 1500) {
                model.remove(0, model.count - 1000)
            }

            model.append({"msg": msg})
            positionViewAtEnd()
        }
    }
}
