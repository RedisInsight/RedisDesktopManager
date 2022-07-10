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
    spacing: 0

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
                                'icon': PlatformUtils.getThemeIcon("offline.svg"), 'event': 'cancel', "help": qsTranslate("RESP","Disconnect"),
                            },
                        ]
            } else {
                return [
                            {
                                'icon': PlatformUtils.getThemeIcon("filter.svg"), 'callback': 'filter', "help": qsTranslate("RESP","Open Keys Filter"),
                                "shortcut": "Ctrl+F",
                            },
                            {
                                'icon': PlatformUtils.getThemeIcon("refresh.svg"), 'event': 'reload', "help": qsTranslate("RESP","Reload Keys in Database"),
                                "shortcut": "Ctrl+R",
                            },
                            {
                                'icon': PlatformUtils.getThemeIcon("add.svg"), 'event': 'add_key', "help": qsTranslate("RESP","Add New Key"),
                                "shortcut": "Ctrl+N",
                            },
                            {
                                'icon': styleData.value["live_update"]? PlatformUtils.getThemeIcon("live_update_disable.svg") : PlatformUtils.getThemeIcon("live_update.svg"),
                                'callback': 'live_update',
                                "help": styleData.value["live_update"]? qsTranslate("RESP","Disable Live Update") : qsTranslate("RESP","Enable Live Update"),
                                "shortcut": "Ctrl+L",
                            },
                            {
                                'icon': PlatformUtils.getThemeIcon("console.svg"), 'event': 'console', "help": qsTranslate("RESP","Open Console"),
                                "shortcut": "Ctrl+T",
                            },
                            {'icon': PlatformUtils.getThemeIcon("memory_usage.svg"), "event": "analyze_memory_usage", "help": qsTranslate("RESP","Analyze Used Memory")},
                            {
                                'icon': PlatformUtils.getThemeIcon("bulk_operations.svg"), 'callback': 'bulk_menu', "help": qsTranslate("RESP","Bulk Operations"),
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
                            'icon': PlatformUtils.getThemeIcon("cleanup.svg"), 'event': 'flush', "help": qsTranslate("RESP","Flush Database"),
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("cleanup_filtered.svg"), 'event': 'delete_keys', "help": qsTranslate("RESP","Delete keys with filter"),
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("ttl.svg"), 'event': 'ttl', "help": qsTranslate("RESP","Set TTL for multiple keys"),
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("db-copy.svg"), 'event': 'copy_keys', "help": qsTranslate("RESP","Copy keys from this database to another"),
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("import.svg"), 'event': 'rdb_import', "help": qsTranslate("RESP","Import keys from RDB file"),
                        },
                        {
                            'icon': PlatformUtils.getThemeIcon("back.svg"), 'callback': 'db_menu', "help": qsTranslate("RESP","Back"),
                        },

                    ]
        }

    }

    RowLayout {
        id: filterMenu

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.topMargin: PlatformUtils.isOSX() ? -3 : 0

        property int btnWidth: PlatformUtils.isOSXRetina(Screen)? 18 : 22
        property int btnHeight: PlatformUtils.isOSXRetina(Screen)? 18 : 22

        BetterComboBox {
            id: filterCombobox
            objectName: "rdm_inline_menu_filter_field"
            editable: true

            Layout.preferredWidth: connectionsTree.width * 0.4
            Layout.preferredHeight: PlatformUtils.isOSX()? 25 : 30

            indicator.width: PlatformUtils.isOSX()? 30 : 40
            indicator.height: PlatformUtils.isOSX()? 25 : 30

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
            iconSource: PlatformUtils.getThemeIcon("ok.svg")
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
            iconSource: PlatformUtils.getThemeIcon("help.svg")
            onClicked: Qt.openUrlExternally("https://docs.resp.app/en/latest/lg-keyspaces/#use-specific-scan-filter-to-reduce-loaded-amount-of-keys")
        }

        ImageButton {
            id: filterCancel

            implicitWidth: filterMenu.btnWidth
            implicitHeight: filterMenu.btnHeight
            imgWidth: filterMenu.btnWidth
            imgHeight: filterMenu.btnHeight
            iconSource: PlatformUtils.getThemeIcon("clear.svg")
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
