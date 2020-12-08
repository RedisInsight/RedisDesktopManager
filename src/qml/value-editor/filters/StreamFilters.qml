import QtQuick 2.13
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import "./../../common"

RowLayout {

    BetterLabel {
        text: qsTranslate("RDM", "Time range:")
    }

    BetterTextField {
        text: streamRangeSlider.first.value
    }

    RangeSlider {
        id: streamRangeSlider
    }

    BetterTextField {
        text: streamRangeSlider.second.value
    }

    Connections {
        target: keyModel? keyModel : null

        function onRowsLoaded() {

            console.log("STREAM filter:",
                        keyModel.filter("first-entry"),
                        keyModel.filter("last-entry"))

            var start = String(keyModel.filter("first-entry")).slice(0, -2)
            var end = String(keyModel.filter("last-entry")).slice(0, -2)

            streamRangeSlider.from = Number(start)
            streamRangeSlider.to = Number(end)
            streamRangeSlider.first.value = Number(start)
            streamRangeSlider.second.value = Number(end)
        }
    }

}
