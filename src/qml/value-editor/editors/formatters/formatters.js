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

    getRaw: function (formatted, callback) {
        return callback("", formatted)
    }
}

var hex = {
    title: "HEX",

    getFormatted: function (raw, callback) {
        return callback("", qmlUtils.printable(raw), false, FORMAT_PLAIN_TEXT)
    },    

    getRaw: function (formatted, callback) {
        return callback("", qmlUtils.printableToValue(formatted))
    }
}

var hexTable = {
    title: "HEX TABLE",

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
            return callback("", JSONFormatter.prettyPrint(String(raw)), false, FORMAT_PLAIN_TEXT)
        } catch (e) {
            return callback(qsTr("Error: Invalid JSON: ") + e)
        }
    },    

    getRaw: function (formatted, callback) {
        try {
            return callback("", JSONFormatter.minify(formatted))
        } catch (e) {
            return callback(qsTr("Error: ") + e)
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

function guessFormatter(isBinary)
{
   return isBinary? 2 : 0
}
