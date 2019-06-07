.import "./hexy.js" as Hexy
.import "./json-tools.js" as JSONFormatter

var FORMAT_PLAIN_TEXT = "plain_text"
var FORMAT_JSON = "json"
var FORMAT_HTML = "html"

/**
  Plain formatter
**/

var plain = {
    title: "Plain Text",

    getFormatted: function (raw, callback) {
        return callback("", raw, false, FORMAT_PLAIN_TEXT)
    },    

    isValid: function (raw, callback) {
        return callback(true)
    },

    getRaw: function (formatted, callback) {
        return callback("", formatted)
    }
}

var hex = {
    title: "HEX",

    getFormatted: function (raw, callback) {
        return callback("", qmlUtils.printable(raw), false, FORMAT_PLAIN_TEXT)
    },    

    isValid: function (raw, callback) {
        return callback(true)
    },

    getRaw: function (formatted, callback) {
        return callback("", qmlUtils.printableToValue(formatted))
    }
}

var hexTable = {
    title: "HEX TABLE",

    isValid: function (raw, callback) {
        return callback(true)
    },

    getFormatted: function (raw, callback) {              
        return callback("", Hexy.hexy(qmlUtils.valueToBinary(raw), {'html': true}), true, FORMAT_HTML)
    },    
}

/**
  JSON formatter
**/
var json = {
    title: "JSON",

    getFormatted: function (raw, callback) {
        try {
            return callback("", JSONFormatter.prettyPrint(String(raw)), false, FORMAT_JSON)
        } catch (e) {
            return callback(qsTranslate("RDM", "Invalid JSON: ") + e)
        }
    },

    isValid: function (raw, callback) {
        try {
            JSONFormatter.prettyPrint(String(raw))
            return callback(true)
        } catch (e) {
            return callback(false)
        }
    },

    getRaw: function (formatted, callback) {
        try {
            var plainText = qmlUtils.htmlToPlainText(formatted)
            return callback("", JSONFormatter.minify(plainText))
        } catch (e) {
            return callback(qsTranslate("RDM", "Error") + ": " + e)
        }
    }
}

var enabledFormatters = [plain, json, hex, hexTable]


function buildFormattersModel()
{
    var formatters = []

    for (var index in enabledFormatters) {
        var f = enabledFormatters[index]
        formatters.push({'name': f.title, 'type': "buildin", "instance": f})
    }   


    function createWrapperForEmbeddedFormatter(formatterName) {
        return {
            getFormatted: function (raw, callback) {
                return embeddedFormattersManager.decode(formatterName, raw, function (response) {
                    return callback(response[0], response[1], response[2], response[3])
                })
            },

            getRaw: function (formatted, callback) {
                return embeddedFormattersManager.encode(formatterName, formatted, callback)
            },

            isValid: function (raw, callback) {
                return embeddedFormattersManager.isValid(formatterName, raw, callback)
            }
        }
    }

    for (var indx in approot.embeddedFormatters) {
        formatters.push({
                   'name': approot.embeddedFormatters[indx],
                   'type': "embedded",
                   'instance': createWrapperForEmbeddedFormatter(approot.embeddedFormatters[indx])
               })
    }

    var nativeFormatters = formattersManager.getPlainList();

    function createWrapperForNativeFormatter(formatterName) {
        return {
            getFormatted: function (raw, callback) {
                return formattersManager.decode(formatterName, raw, callback)
            },

            getRaw: function (formatted, callback) {
                return formattersManager.encode(formatterName, formatted, callback)
            },

            isValid: function (raw, callback) {
                return formattersManager.isValid(formatterName, raw, callback)
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

    return parseInt(indexInNativeFormatters) + enabledFormatters.length

}

function guessFormatter(isBinary)
{
    if (isBinary) {
        if (formattersManager.isInstalled("python-decompresser")) {
            return [getFormatterIndex("python-decompresser"), 2]
        }

        return 2
    } else {
        return 0
    }
}
