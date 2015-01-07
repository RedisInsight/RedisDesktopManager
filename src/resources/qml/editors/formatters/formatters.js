.import "./msgpack.js" as MsgPack

function get(type) {

    if (type === "plain") return plain
    if (type === "json") return json
    if (type === "msgpack") return msgpack
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
        return formatted
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
        obj = JSON.parse(formatted)
        return MsgPack.msgpack().pack(obj)
    }
}
