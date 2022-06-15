import QtQuick 2.0
import QtQml.Models 2.13
import "./hexy.js" as Hexy
import "../../../common/platformutils.js" as PlatformUtils

ListModel {
    id: rootModel

    property int _jsonFormatterIndex: 3

    function guessFormatter(val, isBinary)
    {       
        if (isBinary) {
            return 1
        } else {
            if (qmlUtils.isJSON(val)) {
                return _jsonFormatterIndex
            } else {
                return 0
            }
        }

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
        return rootModel.get(_jsonFormatterIndex)
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

    function onEmbeddedFormattersLoaded(result) {
        for (var indx in result) {
            var formatterName = result[indx][0];
            var readOnly = result[indx][1];

            var getFormatted = function (formatterName) {
                var r = function (raw, callback, context) {
                    return embeddedFormattersManager.decode(formatterName, raw, function (response) {
                        return callback(response[0], response[1], response[2], response[3])
                    })
                }
                return r
            };

            var getRaw = function (formatterName) {
                var r = function (formatted, callback, context) {
                    return embeddedFormattersManager.encode(formatterName, formatted, function (response) {
                        return callback(response[0], response[1])
                    })
                }
                return r
            };

            var isValid = function (formatterName, context) {
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
            rootModel.setProperty(rootModel.count - 1, "keyTypes", "*")

        }

        console.log("Embedded formatters:", result);
    }

    function loadEmbeddedFormatters()
    {
        embeddedFormattersManager.loadFormattersModule(function (result) {
            console.log("Is Embedded formatters module loaded:", result)

            if (!result) {
                return;
            }

            embeddedFormattersManager.loadFormatters(function (result) {
                rootModel.onEmbeddedFormattersLoaded(result);
            });
        })
    }


    function loadExternalFormatters() {
        var nativeFormatters = formattersManager.getPlainList();

        for (var index in nativeFormatters) {
            var formatter = nativeFormatters[index];

            var formatterName = formatter["name"];
            var formatterId = formatter["id"];
            var readOnly = formatter["readOnly"];

            var getFormatted = function (formatterId) {
                var r = function (raw, callback, context) {
                    return formattersManager.decode(formatterId, raw, context, callback)
                }
                return r
            };

            var getRaw = function (formatterId) {
                var r = function (formatted, callback, context) {
                    return formattersManager.encode(formatterId, formatted, context, callback)
                }
                return r
            };

            var isValid = function (formatterId, context) {
                var r = function (raw, callback) {
                    return formattersManager.isValid(formatterId, raw, context, callback)
                }
                return r
            };

            rootModel.append({'name': formatterName, 'type': "external"})
            rootModel.setProperty(rootModel.count - 1, "getFormatted", getFormatted(formatterId))
            rootModel.setProperty(rootModel.count - 1, "getRaw", getRaw(formatterId))
            rootModel.setProperty(rootModel.count - 1, "isValid", isValid(formatterId))
            rootModel.setProperty(rootModel.count - 1, "readOnly", readOnly)
        }
    }

    function updateRWFormatters() {
        var result = [];

        for (var index=0; index < rootModel.count; ++index) {
            var formatter = get(index);

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

        property string keyTypes: "*"

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

        property string keyTypes: "*"

        property var getFormatted: function (raw, callback, context) {
            return callback("", qmlUtils.printable(raw), false, "plain")
        }

        property var isValid: function (raw, callback, context) {
            return callback(true)
        }

        property var getRaw: function (formatted, callback, context) {
            return callback("", qmlUtils.printableToValue(formatted))
        }
    }

    ListElement {
        property string name: "HEX TABLE"

        property string type: "buildin"

        property string readOnly: true

        property string keyTypes: "*"

        property var isValid: function (raw, callback, context) {
            return callback(true)
        }

        property var getFormatted: function (raw, callback, context) {
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

        property string keyTypes: "*"

        property var getFormatted: function (raw, callback, context) {
            return callback("", qmlUtils.prettyPrintJSON(raw), false, "json")
        }

        property var isValid: function (raw, callback, context) {
            return callback(qmlUtils.isJSON(raw))
        }

        property var getRaw: function (formatted, callback, context) {
            var minified = qmlUtils.minifyJSON(formatted);

            if (!minified) {
                return callback(qsTranslate("RESP", "Error") + ": Cannot minify JSON string")
            } else {
                return callback("", minified)
            }
        }
    }

    ListElement {
        property string name: "BASE64 to Text"

        property string type: "buildin"

        property string readOnly: true

        property string keyTypes: "*"

        property var getFormatted: function (raw, callback, context) {
            return callback("", Qt.atob(raw), false, "plain")
        }

        property var isValid: function (raw, callback, context) {
            return callback(true)
        }

        property var getRaw: function (formatted, callback, context) {
            return callback("", Qt.btoa(formatted))
        }
    }

    ListElement {
        property string name: "BASE64 to JSON"

        property string type: "buildin"

        property string readOnly: true

        property string keyTypes: "*"

        property var getFormatted: function (raw, callback, context) {
            return callback("", Qt.atob(raw), false, "json")
        }

        property var isValid: function (raw, callback, context) {
            return callback(true)
        }

        property var getRaw: function (formatted, callback, context) {
            return callback("", Qt.btoa(formatted))
        }
    }
}
