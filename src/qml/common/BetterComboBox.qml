import QtQuick 2.13
import QtQuick.Controls 2.13

ComboBox {
    id: root
    implicitHeight: 30

    palette.base: sysPalette.base
    palette.button: sysPalette.button
    palette.text: sysPalette.text
    palette.buttonText: sysPalette.text
    palette.highlightedText: sysPalette.buttonText
    palette.highlight: sysPalette.highlight
    palette.mid: sysPalette.mid
    palette.dark: sysPalette.dark
    palette.window: sysPalette.window

    function selectItem(txt) {
        var index = find(txt)

        console.log("Index:", index)

        if (index !== -1) {
            currentIndex = index;
        }

        activated(index)
    }
}
