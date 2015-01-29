.pragma library
.import "./msgpack.js" as MsgPack
.import "./hexy.js" as Hexy
.import "./php-unserialize.js" as PHPUnserialize
.import "./php-serialize.js" as PHPSerialize

var defaultFormatter = 0;

function get(type) {

    if (type === "plain") return plain
    if (type === "hex") return hex
    if (type === "json") return json
    if (type === "msgpack") return msgpack
    if (type === "php-serialized") return phpserialized
}


/**
  Plain formatter
**/

var plain = {

    readOnly: false,

    getFormatted: function (raw) {
        return raw
    },

    isValid: function (raw) {
        return true
    },

    getRaw: function (formatted) {
        return formatted
    }
}

var hex = {
    readOnly: true,

    getFormatted: function (raw) {
        var format = {'html': true}
        return Hexy.hexy(raw, format)
    },

    isValid: function (raw) {
        return true
    },

    getRaw: function (formatted) {
        return ''
    }
}

/**
  JSON formatter
**/
var json = {

    readOnly: false,

    getFormatted: function (raw) {

        try {
            var parsed = JSON.parse(raw)
            return JSON.stringify(parsed, undefined, 4)

        } catch (e) {
            return "Error: Invalid JSON"
        }
    },

    isValid: function (raw) {
        try {
            JSON.parse(raw)
            return true
        } catch (e) {
            return false
        }
    },

    getRaw: function (formatted) {        
        try {
            var parsed = JSON.parse(formatted)
            return JSON.stringify(parsed)

        } catch (e) {
            return formatted
        }
    }
}

/**
  MsgPack formatter
**/
var msgpack = {

    readOnly: false,

    getFormatted: function (raw) {

        try {
            var parsed = MsgPack.msgpack().unpack(raw)
            console.log('parsed msgpack:', parsed)
            return JSON.stringify(parsed, undefined, 4)

        } catch (e) {
            return "Error: Invalid MSGPack or JSON" + e
        }
    },

    isValid: function (raw) {
        try {
            MsgPack.msgpack().unpack(raw)
            return true
        } catch (e) {
            return false
        }
    },

    getRaw: function (formatted) {
        var obj = JSON.parse(formatted)
        var compressed = MsgPack.msgpack().pack(obj, true)
        console.log('compressed: ', compressed)
        return compressed
    }
}

/**
  PHP Serialize formatter
**/
var phpserialized = {

    readOnly: false,

    getFormatted: function (raw) {

        try {
            var parsed = PHPUnserialize.unserialize(raw)
            console.log('parsed php serialized:', parsed)
            return JSON.stringify(parsed, undefined, 4)

        } catch (e) {
            return "Error: Invalid PHP Serialized String: " + e
        }
    },

    isValid: function (raw) {
        try {
            MsgPack.msgpack().unpack(raw)
            return true
        } catch (e) {
            return false
        }
    },

    getRaw: function (formatted) {
        var obj = JSON.parse(formatted)
        return PHPSerialize.serialize(obj)
    }
}
