import QtQuick 2.0
import QtQml.Models 2.13
import "./hexy.js" as Hexy
import "./json-tools.js" as JSONFormatter
import "../../../common/platformutils.js" as PlatformUtils

ListModel {
    id: rootModel

    function guessFormatter(isBinary)
    {
        return getDefaultFormatter(isBinary)
    }

    function getDefaultFormatter(isBinary)
    {
        if (isBinary) {
            return 1
        } else {
            return 0
        }
    }

    function getJSONFormatter() {
        return rootModel.get(3)
    }

    property var rwFormatters

    function getFormatterIndex(name) {
        for (var index=0; index < rootModel.count; ++index) {
            var formatter = get(index);

            if (formatter['name'] == name) {
                return index;
            }
        }

        return 0;
    }

    function loadEmbeddedFormatters()
    {
        embeddedFormattersManager.loadFormatters(function (result) {
            for (var indx in result) {
                var formatterName = result[indx][0];
                var readOnly = result[indx][1];

                var getFormatted = function (formatterName) {
                    var r = function (raw, callback) {
                        return embeddedFormattersManager.decode(formatterName, raw, function (response) {
                            return callback(response[0], response[1], response[2], response[3])
                        })
                    }
                    return r
                };

                var getRaw = function (formatterName) {
                    var r = function (formatted, callback) {
                        return embeddedFormattersManager.encode(formatterName, formatted, function (response) {
                            return callback(response[0], response[1])
                        })
                    }
                    return r
                };

                var isValid = function (formatterName) {
                    var r = function (raw, callback) {
                        return embeddedFormattersManager.isValid(formatterName, raw, function (response) {
                            return callback(response[0])
                        })
                    }
                    return r
                };

                rootModel.append({'name': formatterName, 'type': "embedded",})
                rootModel.setProperty(rootModel.count - 1, "getFormatted", getFormatted(formatterName))
                rootModel.setProperty(rootModel.count - 1, "getRaw", getRaw(formatterName))
                rootModel.setProperty(rootModel.count - 1, "isValid", isValid(formatterName))
                rootModel.setProperty(rootModel.count - 1, "readOnly", readOnly)

            }

            console.log("Embedded formatters:", result);
        });
    }


    function loadExternalFormatters() {
        var nativeFormatters = formattersManager.getPlainList();

        for (var index in nativeFormatters) {
            var formatter = nativeFormatters[index];

            var formatterName = formatter["name"];
            var readOnly = formatter["read_only"];

            var getFormatted = function (formatterName) {
                var r = function (raw, callback) {
                    return formattersManager.decode(formatterName, raw, callback)
                }
                return r
            };

            var getRaw = function (formatterName) {
                var r = function (formatted, callback) {
                    return formattersManager.encode(formatterName, formatted, callback)
                }
                return r
            };

            var isValid = function (formatterName) {
                var r = function (raw, callback) {
                    return formattersManager.isValid(formatterName, raw, callback)
                }
                return r
            };

            rootModel.append({'name': formatterName, 'type': "external"})
            rootModel.setProperty(rootModel.count - 1, "getFormatted", getFormatted(formatterName))
            rootModel.setProperty(rootModel.count - 1, "getRaw", getRaw(formatterName))
            rootModel.setProperty(rootModel.count - 1, "isValid", isValid(formatterName))
            rootModel.setProperty(rootModel.count - 1, "readOnly", readOnly)
        }
    }

    function updateRWFormatters() {
        var result = [];

        for (var index=0; index < rootModel.count; ++index) {
            var formatter = get(index);

            console.log(formatter, formatter['name'], formatter['readOnly'])

            if (formatter['readOnly'] == false) {
                result.push(formatter);
            }
        }

        rwFormatters = result;
    }

    ListElement {
        property string name: "Plain Text"

        property string type: "buildin"

        property string readOnly: false

        property var getFormatted: function (raw, callback) {
            return callback("", raw, false, "plain")
        }

        property var isValid: function (raw, callback) {
            return callback(true)
        }

        property var getRaw: function (formatted, callback) {
            return callback("", formatted)
        }
    }

    ListElement {
        property string name: "HEX"

        property string type: "buildin"

        property string readOnly: true

        property var getFormatted: function (raw, callback) {
            return callback("", qmlUtils.printable(raw), false, "plain")
        }

        property var isValid: function (raw, callback) {
            return callback(true)
        }

        property var getRaw: function (formatted, callback) {
            return callback("", qmlUtils.printableToValue(formatted))
        }
    }

    ListElement {
        property string name: "HEX TABLE"

        property string type: "buildin"

        property string readOnly: true

        property var isValid: function (raw, callback) {
            return callback(true)
        }

        property var getFormatted: function (raw, callback) {
            return callback("", Hexy.hexy(
                                qmlUtils.valueToBinary(raw),
                                {'html': true, 'font': appSettings.valueEditorFont}),
                            true, "html")
        }
    }

    ListElement {
        property string name: "JSON"

        property string type: "buildin"

        property string readOnly: false

        property var getFormatted: function (raw, callback) {
            console.error("Call JSON worker script")
        }

        property var isValid: function (raw, callback) {
            console.error("Call JSON worker script")
        }

        property var getRaw: function (formatted, callback) {
            try {
                var plainText = qmlUtils.htmlToPlainText(formatted)
                return callback("", JSONFormatter.minify(plainText))
            } catch (e) {
                return callback(qsTranslate("RDM", "Error") + ": " + e)
            }
        }
    }

    ListElement {
        property string name: "BASE64 to Text"

        property string type: "buildin"

        property string readOnly: true

        property var getFormatted: function (raw, callback) {
            return callback("", Qt.atob(raw), false, "plain")
        }

        property var isValid: function (raw, callback) {
            return callback(true)
        }

        property var getRaw: function (formatted, callback) {
            return callback("", Qt.btoa(formatted))
        }
    }

    ListElement {
        property string name: "BASE64 to JSON"

        property string type: "buildin"

        property string readOnly: true

        property var getFormatted: function (raw, callback) {
            return callback("", Qt.atob(raw), false, "json")
        }

        property var isValid: function (raw, callback) {
            return callback(true)
        }

        property var getRaw: function (formatted, callback) {
            return callback("", Qt.btoa(formatted))
        }
    }
}
