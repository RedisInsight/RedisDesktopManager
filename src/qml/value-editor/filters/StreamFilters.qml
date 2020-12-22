import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import "./../../common"
import "../../common/platformutils.js" as PlatformUtils

RowLayout {
    id: streamFilter
    objectName: "rdm_stream_filter"

    property bool enabled: streamRangeSlider.from < streamRangeSlider.to
    property string dateTimeFormat: "yyyy-MM-dd hh:mm:ss.zzz"
    property string inputMask: "9999-99-99 99:99:99.999"

    function setStreamFilter() {
        var start = Date.fromLocaleString(locale, streamRangeStartField.text, streamFilter.dateTimeFormat).getTime()
        var end = Date.fromLocaleString(locale, streamRangeEndField.text, streamFilter.dateTimeFormat).getTime()
        if (start < end) {
            keyTab.keyModel.setFilter("start", start)
            keyTab.keyModel.setFilter("end", end)

            reloadValue()

            streamRangeStartField.isEdited = false;
            streamRangeEndField.isEdited = false;
        } else {
            notification.showError(qsTranslate("RDM","Start date should be less than End date"))
        }
    }

    RegExpValidator {
        id: dateTimeValidator
        regExp: /(\d{4})-(\d{2})-(\d{2}) (\d{2})\:(\d{2})\:(\d{2}).(\d{3})/
    }

    BetterTextField {
        id: streamRangeStartField
        objectName: "rdm_stream_filter_start_field"

        property bool isEdited: false

        implicitWidth: 180
        font.pixelSize: 14
        color: enabled ? sysPalette.text : disabledSysPalette.text

        enabled: streamFilter.enabled

        text: new Date(streamRangeSlider.first.value).toLocaleString(locale, streamFilter.dateTimeFormat)
        inputMask: streamFilter.inputMask
        validator: dateTimeValidator

        BetterToolTip {
            title: streamRangeSlider.first.value
            visible: title && (streamRangeSlider.first.pressed || streamRangeSlider.first.hovered)
        }

        onTextEdited: {
            isEdited = true
        }

        onAccepted: {
            streamFilter.setStreamFilter()
        }
    }

    RangeSlider {
        id: streamRangeSlider
        objectName: "rdm_stream_filter_range_slider"

        Layout.fillWidth: true
        palette.midlight: sysPalette.button
        palette.dark: enabled ? sysPalette.highlight : disabledSysPalette.highlight
        padding: 0

        enabled: streamFilter.enabled

        stepSize: 1.0
        snapMode: Slider.SnapAlways

        first.handle.implicitWidth: 20
        first.handle.implicitHeight: 20

        second.handle.implicitWidth: 20
        second.handle.implicitHeight: 20

        first.onPressedChanged: {
            if (!first.pressed) {
                streamRangeStartField.isEdited = true
            }
        }

        second.onPressedChanged: {
            if (!second.pressed) {
                streamRangeEndField.isEdited = true
            }
        }
    }

    BetterTextField {
        id: streamRangeEndField
        objectName: "rdm_stream_filter_end_field"

        property bool isEdited: false

        implicitWidth: 180
        font.pixelSize: 14
        color: enabled ? sysPalette.text : disabledSysPalette.text

        enabled: streamFilter.enabled

        text: new Date(streamRangeSlider.second.value).toLocaleString(locale, streamFilter.dateTimeFormat)
        inputMask: streamFilter.inputMask
        validator: dateTimeValidator

        BetterToolTip {
            title: streamRangeSlider.second.value
            visible: title && (streamRangeSlider.second.pressed || streamRangeSlider.second.hovered)
        }

        onTextEdited: {
            isEdited = true
        }

        onAccepted: {
            streamFilter.setStreamFilter()
        }
    }

    BetterButton {
        objectName: "rdm_stream_filter_apply_btn"

        iconSource: "qrc:/images/filter.svg"
        text: qsTranslate("RDM","Apply filter")
        enabled: (streamRangeStartField.isEdited || streamRangeEndField.isEdited) && streamFilter.enabled

        onClicked: {
            streamFilter.setStreamFilter()
        }
    }

    Connections {
        target: keyModel ? keyModel : null

        function onRowsLoaded() {

            var firstEntry = String(keyModel.filter("first-entry")).slice(0, -2)
            var lastEntry = String(keyModel.filter("last-entry")).slice(0, -2)

            var start = keyModel.filter("start") ? keyModel.filter("start") : firstEntry
            var end = keyModel.filter("end") ? keyModel.filter("end") : lastEntry

            console.log("STREAM filter start end:", start, end)

            streamRangeSlider.from = Number(firstEntry)
            streamRangeSlider.to = Number(lastEntry)

            streamRangeSlider.first.value = Number(start)
            streamRangeSlider.second.value = Number(end)
        }
    }

}
