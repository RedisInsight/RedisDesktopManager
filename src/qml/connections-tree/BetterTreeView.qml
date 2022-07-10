import QtQuick 2.14
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.2
import QtQuick.Window 2.2
import "./../common/platformutils.js" as PlatformUtils
import "./../common"
import "."

TreeView {
    id: root
    alternatingRowColors: false
    headerVisible: false
    focus: true
    horizontalScrollBarPolicy: Qt.ScrollBarAsNeeded
    verticalScrollBarPolicy: Qt.ScrollBarAsNeeded

    model: connectionsManager

    property bool sortConnections: false

    backgroundVisible: false

    /**
      * NOTE(u_glide): Dirty hack to use build-in macOS style for scrollbars on all platforms
      */
    Component.onCompleted: {
        if (!PlatformUtils.isOSX()) {
            __scroller.verticalScrollBar.__panel.on = true
        }
    }

    Connections {
        target: !PlatformUtils.isOSX()? __scroller.verticalScrollBar.__panel : null

        function onOnChanged() {
            if (!__scroller.verticalScrollBar.__panel.on) {
                __scroller.verticalScrollBar.__panel.on = true
            }
        }
    }

    Component {
        id: patchedBackground

        Item {
            implicitWidth: 25
            implicitHeight: 200
        }
    }
    // hack-end

    style: TreeViewStyle {
        frame: Item {}

        indentation: 12

        rowDelegate: Rectangle {
            height: PlatformUtils.isOSXRetina(Screen) ? 25 : 30
            color: styleData.selected ? sysPalette.highlight : "transparent"
        }

        transientScrollBars: true

        backgroundColor: sysPalette.button

        scrollBarBackground: PlatformUtils.isOSX()? TreeViewStyle.scrollBarBackground : patchedBackground
    }

    TableViewColumn {
        id: itemColumn
        title: "item"
        role: "metadata"

        delegate: TreeItemDelegate {
            id: itemRoot            
            treeRoot: root
            sortConnections: root.sortConnections
        }
    }    

    selectionMode: SelectionMode.SingleSelection

    selection: ItemSelectionModel {
        id: connectionTreeSelectionModel
        model: connectionsManager
    }

    onClicked: connectionsManager && connectionsManager.sendEvent(index, "click")
    onExpanded: connectionsManager.setExpanded(index)
    onCollapsed: connectionsManager.setCollapsed(index)

    Connections {
        target: connectionsManager

        function onExpand(index) {
            if (root.isExpanded(index))
                return

            root.expand(index)
        }
    }
}
