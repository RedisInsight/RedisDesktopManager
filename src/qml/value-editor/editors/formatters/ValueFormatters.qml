import QtQuick 2.0
import QtQml.Models 2.13
import "./hexy.js" as Hexy
import "./json-tools.js" as JSONFormatter

ListModel {
    id: rootModel

    function guessFormatter(isBinary)
    {
        if (isBinary) {
            return 2
        } else {
            return 0
        }
    }

    function getFormatterIndex(name) {
        var indexInNativeFormatters = -1

        if (!formattersManager.isInstalled(name))
            return indexInNativeFormatters

        var plainList = formattersManager.getPlainList()

        for (var index in plainList) {
            if (plainList[index] === name) {
                indexInNativeFormatters = index
                break
            }
        }

        return parseInt(indexInNativeFormatters)
    }

    function loadEmbeddedFormatters()
    {
        embeddedFormattersManager.loadFormatters(function (result) {
            for (var indx in result) {
                var formatterName = result[indx];

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
                };

                var isValid = function (formatterName) {
                    var r = function (raw, callback) {
                        return embeddedFormattersManager.isValid(formatterName, raw, function (response) {
                            return callback(response[0])
                        })
                    }
                }

                rootModel.append({'name': formatterName, 'type': "embedded",})
                rootModel.setProperty(rootModel.count - 1, "getFormatted", getFormatted(formatterName))
                rootModel.setProperty(rootModel.count - 1, "getRaw", getRaw(formatterName))
                rootModel.setProperty(rootModel.count - 1, "isValid", isValid(formatterName))
            }

            console.log("Embedded formatters:", result);
        });
    }


    function loadExternalFormatters() {
        var nativeFormatters = formattersManager.getPlainList();

        for (var index in nativeFormatters) {
            var formatterName = nativeFormatters[index];

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
            };

            var isValid = function (formatterName) {
                var r = function (raw, callback) {
                    return formattersManager.isValid(formatterName, raw, callback)
                }
            }

            rootModel.append({'name': formatterName, 'type': "external"})
            rootModel.setProperty(rootModel.count - 1, "getFormatted", getFormatted(formatterName))
            rootModel.setProperty(rootModel.count - 1, "getRaw", getRaw(formatterName))
            rootModel.setProperty(rootModel.count - 1, "isValid", isValid(formatterName))
        }
    }

    ListElement {
        property string name: "Plain Text"

        property string type: "buildin"

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

        property var isValid: function (raw, callback) {
            return callback(true)
        }

        property var getFormatted: function (raw, callback) {
            return callback("", Hexy.hexy(qmlUtils.valueToBinary(raw), {'html': true}), true, "html")
        }
    }

    ListElement {
        property string name: "JSON"

        property string type: "buildin"

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
}
