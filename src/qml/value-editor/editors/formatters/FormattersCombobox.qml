import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import "./formatters.js" as Formatters


ComboBox {
    id: control
    textRole: "name"
    model: buildFormattersModel()

    delegate: ItemDelegate {
        width: control.width
        contentItem: RowLayout {
            id: contentItem
            anchors.fill: parent

            property var parts: modelData[control.textRole].split("-", 1)

            Image {
                visible: contentItem.parts && contentItem.parts.length > 1
                source: "qrc://" + contentItem.parts[0] + ".svg"
            }

            Label {
                Layout.fillWidth: true
                text: contentItem.parts.length > 1 ? contentItem.parts[1] : modelData[control.textRole]
            }
        }

        highlighted: control.highlightedIndex === index
    }

    function buildFormattersModel()
    {
        var formatters = []

        for (var index in Formatters.enabledFormatters) {
            var f = Formatters.enabledFormatters[index]
            formatters.push({'name': f.title, 'type': "buildin", "instance": f})
        }

        var nativeFormatters = formattersManager.getPlainList();

        function createWrapperForNativeFormatter(formatterName) {
            return {
                getFormatted: function (raw, callback) {
                    return formattersManager.decode(formatterName, raw, callback)
                },

                getRaw: function (formatted, callback) {
                    return formattersManager.encode(formatterName, formatted, callback)
                }
            }
        }

        for (var index in nativeFormatters) {
            formatters.push({
                       'name': nativeFormatters[index],
                       'type': "external",
                       'instance': createWrapperForNativeFormatter(nativeFormatters[index])
                   })
        }

        return formatters
    }
}
