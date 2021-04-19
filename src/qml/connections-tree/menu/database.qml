import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.13
import QtQuick.Window 2.2
import "./../../common/platformutils.js" as PlatformUtils
import "."
import "./../../common/"

RowLayout {
    id: root

    focus: true

    state: "menu"

    states: [
        State {
            name: "menu"
            PropertyChanges { target: dbMenu; visible: true;}
            PropertyChanges { target: bulkMenu; visible: false;}
            PropertyChanges { target: filterMenu; visible: false;}            
        },
        State {
            name: "bulk_menu"
            PropertyChanges { target: dbMenu; visible: false;}
            PropertyChanges { target: bulkMenu; visible: true;}
            PropertyChanges { target: filterMenu; visible: false;}
        },
        State {
            name: "filter"
            PropertyChanges { target: dbMenu; visible: false;}
            PropertyChanges { target: bulkMenu; visible: false;}
            PropertyChanges { target: filterMenu; visible: true;}
        }
    ]

    Keys.onPressed: {
        if (state == "filter" && event.key == Qt.Key_Escape) {
            state = "menu"
        }
    }

    InlineMenu {
        id: dbMenu

        Layout.fillWidth: true

        callbacks: {
            "filter": function() {
                root.state = "filter"
                filterCombobox.currentIndex = filterCombobox.find(styleData.value["filter"])
                filterCombobox.editText = styleData.value["filter"]
            },
            "live_update": function () {
                if (styleData.value["live_update"]) {
                    connectionsManager.setMetadata(styleData.index, "live_update", '')
                } else {
                    connectionsManager.setMetadata(styleData.index, "live_update", true)
                }
            },
            "bulk_menu": function() {
                root.state = "bulk_menu"
            },
        }

        model: {
            if (styleData.value["locked"] === true) {
                return [
                            {
                                'icon': "qrc:/images/offline.svg", 'event': 'cancel', "help": qsTranslate("RDM","Disconnect"),
                            },
                        ]
            } else {
                return [
                            {
                                'icon': "qrc:/images/filter.svg", 'callback': 'filter', "help": qsTranslate("RDM","Open Keys Filter"),
                                "shortcut": PlatformUtils.isOSX()? "Meta+F" : "Ctrl+F",
                            },
                            {
                                'icon': "qrc:/images/refresh.svg", 'event': 'reload', "help": qsTranslate("RDM","Reload Keys in Database"),
                                "shortcut": PlatformUtils.isOSX()? "Meta+R" : "Ctrl+R",
                            },
                            {
                                'icon': "qrc:/images/add.svg", 'event': 'add_key', "help": qsTranslate("RDM","Add New Key"),
                                "shortcut": PlatformUtils.isOSX()? "Meta+N" : "Ctrl+N",
                            },
                            {
                                'icon': styleData.value["live_update"]? "qrc:/images/live_update_disable.svg" : "qrc:/images/live_update.svg",
                                'callback': 'live_update',
                                "help": styleData.value["live_update"]? qsTranslate("RDM","Disable Live Update") : qsTranslate("RDM","Enable Live Update"),
                                "shortcut": PlatformUtils.isOSX()? "Meta+L" : "Ctrl+L",
                            },
                            {
                                'icon': "qrc:/images/console.svg", 'event': 'console', "help": qsTranslate("RDM","Open Console"),
                                "shortcut": Qt.platform.os == "osx"? "Meta+T" : "Ctrl+T",
                            },
                            {'icon': "qrc:/images/memory_usage.svg", "event": "analyze_memory_usage", "help": qsTranslate("RDM","Analyze Used Memory")},
                            {
                                'icon': "qrc:/images/bulk_operations.svg", 'callback': 'bulk_menu', "help": qsTranslate("RDM","Bulk Operations"),
                            },
                        ]
            }
        }
    }

    InlineMenu {
        id: bulkMenu

        Layout.fillWidth: true

        callbacks: {
            "db_menu": function() {
                root.state = "menu"
            },
        }

        model: {
            return [
                        {
                            'icon': "qrc:/images/cleanup.svg", 'event': 'flush', "help": qsTranslate("RDM","Flush Database"),                            
                        },
                        {
                            'icon': "qrc:/images/cleanup_filtered.svg", 'event': 'delete_keys', "help": qsTranslate("RDM","Delete keys with filter"),
                        },
                        {
                            'icon': "qrc:/images/ttl.svg", 'event': 'ttl', "help": qsTranslate("RDM","Set TTL for multiple keys"),
                        },
                        {
                            'icon': "qrc:/images/db_copy.svg", 'event': 'copy_keys', "help": qsTranslate("RDM","Copy keys from this database to another"),
                        },
                        {
                            'icon': "qrc:/images/import.svg", 'event': 'rdb_import', "help": qsTranslate("RDM","Import keys from RDB file"),
                        },
                        {
                            'icon': "qrc:/images/back.svg", 'callback': 'db_menu', "help": qsTranslate("RDM","Back"),
                        },

                    ]
        }

    }

    RowLayout {
        id: filterMenu

        Layout.fillWidth: true

        property int btnWidth: PlatformUtils.isOSXRetina(Screen)? 18 : 22
        property int btnHeight: PlatformUtils.isOSXRetina(Screen)? 18 : 22

        BetterComboBox {
            id: filterCombobox
            objectName: "rdm_inline_menu_filter_field"
            editable: true
            Layout.fillWidth: true
            indicator.width: 30
            selectTextByMouse: true
            editText: styleData.value["filter"]
            model: styleData.value["filterHistory"]

            palette.highlightedText: sysPalette.highlightedText

            delegate: ItemDelegate {
                height: filterCombobox.height
                width: filterCombobox.width
                highlighted: filterCombobox.highlightedIndex === index
                contentItem: Text {
                    text: modelData
                    color: parent.highlighted ? sysPalette.buttonText : sysPalette.text
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight

                    BetterToolTip {
                        title: modelData
                        visible: parent.truncated && title && hovered
                    }
                }
            }

            onAccepted: {
                filterOk.setFilter()
                focus = false
            }
        }

        ImageButton {
            id: filterOk

            implicitWidth: filterMenu.btnWidth
            implicitHeight: filterMenu.btnHeight
            imgWidth: filterMenu.btnWidth
            imgHeight: filterMenu.btnHeight
            iconSource: "qrc:/images/ok.svg"
            objectName: "rdm_inline_menu_button_apply_filter"

            onClicked: setFilter()

            function setFilter() {
                if (!connectionsManager)
                    return

                connectionsManager.setMetadata(styleData.index, "filter", filterCombobox.editText)
                root.state = "menu"
            }
        }

        ImageButton {
            id: filterHelp

            implicitWidth: filterMenu.btnWidth
            implicitHeight: filterMenu.btnHeight
            imgWidth: filterMenu.btnWidth
            imgHeight: filterMenu.btnHeight
            iconSource: "qrc:/images/help.svg"
            onClicked: Qt.openUrlExternally("http://docs.rdm.dev/en/latest/features/#search-in-connection-tree")
        }

        ImageButton {
            id: filterCancel

            implicitWidth: filterMenu.btnWidth
            implicitHeight: filterMenu.btnHeight
            imgWidth: filterMenu.btnWidth
            imgHeight: filterMenu.btnHeight
            iconSource: "qrc:/images/clear.svg"
            objectName: "rdm_inline_menu_button_reset_filter"

            onClicked: {
                if (!connectionsManager)
                    return

                connectionsManager.setMetadata(styleData.index, "filter", "")
                root.state = "menu"
            }
        }
    }
}
