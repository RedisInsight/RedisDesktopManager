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
        return callback(raw, false, FORMAT_PLAIN_TEXT)
    },

    isValid: function (raw, callback) {
        return callback(true, "")
    },

    getRaw: function (formatted, callback) {
        return callback(formatted)
    }
}

var hex = {
    title: "HEX",

    getFormatted: function (raw, callback) {
        return callback(binaryUtils.printable(raw), false, FORMAT_PLAIN_TEXT)
    },

    isValid: function (raw, callback) {
        return callback(binaryUtils.isBinaryString(raw), "")
    },

    getRaw: function (formatted, callback) {
        return callback(binaryUtils.printableToValue(formatted))
    }
}

var hexTable = {
    title: "HEX TABLE",

    getFormatted: function (raw, callback) {        
        return callback(Hexy.hexy(binaryUtils.valueToBinary(raw), {'html': true}), true, FORMAT_HTML)
    },

    isValid: function (raw, callback) {
        return callback(binaryUtils.isBinaryString(raw), "")
    },
}

/**
  JSON formatter
**/
var json = {
    title: "JSON",

    getFormatted: function (raw, callback) {
        try {
            return callback(JSONFormatter.prettyPrint(raw), false, FORMAT_PLAIN_TEXT)

        } catch (e) {
            return callback("Error: Invalid JSON")
        }
    },

    isValid: function (raw, callback) {
        try {
            JSON.parse(raw)
            return callback(true)
        } catch (e) {
            return callback(false)
        }
    },

    getRaw: function (formatted, callback) {
        try {
            return callback(JSONFormatter.minify(formatted))
        } catch (e) {
            return callback(formatted)
        }
    }
}

var defaultFormatterIndex = 0;                        
var enabledFormatters = [plain, json, hex, hexTable]


function buildFormattersModel()
{
    var formatters = []

    for (var index in enabledFormatters) {
        var f = enabledFormatters[index]
        formatters.push({'name': f.title, 'type': "buildin", "instance": f})
    }

    var nativeFormatters = formattersManager.getPlainList();

    function createWrapperForNativeFormatter(formatterName) {
        return {
            getFormatted: function (raw, callback) {
                return formattersManager.decode(formatterName, raw, callback)
            },

            isValid: function (raw, callback) {
                return formattersManager.isValid(formatterName, raw, callback)
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

function guessFormatter(isBinary)
{    
   return isBinary? 2 : 0
}
