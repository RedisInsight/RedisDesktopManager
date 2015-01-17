.import "./msgpack.js" as MsgPack
.import "./php-unserialize.js" as PHPUnserialize
.import "./php-serialize.js" as PHPSerialize

function get(type) {

    if (type === "plain") return plain
    if (type === "json") return json
    if (type === "msgpack") return msgpack
    if (type === "php-serialized") return phpserialized
}


/**
  Plain formatter
**/

var plain = {

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

/**
  JSON formatter
**/
var json = {
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
