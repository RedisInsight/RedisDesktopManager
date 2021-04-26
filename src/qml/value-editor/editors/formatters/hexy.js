// BASED ON: = hexy.js -- utility to create hex dumps
// http://github.com/a2800276/hexy.js

var hexy = function (buffer, config) {
    var h = new Hexy(buffer, config)
    return h.toString()
}
var Hexy = function (buffer, config) {
    var self = {}
    buffer = buffer
    config = config || {}
    self.buffer = buffer // magic string conversion here?
    self.width = config.width || 16
    self.numbering = config.numbering == "none" ? "none" : "hex_bytes"
    switch (config.format) {
    case "none":
    case "twos":
        self.format = config.format
        break
    default:
        self.format = "fours"
    }
    self.caps = config.caps == "upper" ? "upper" : "lower"
    self.annotate = config.annotate == "none" ? "none" : "ascii"
    self.prefix = config.prefix || ""
    self.indent = config.indent || 0
    self.html = config.html || false
    self.offset = config.offset || 0
    self.length = config.length || -1
    self.display_offset = config.display_offset || 0
    self.font = config.font || "monospace"
    if (self.offset) {
        if (self.offset < self.buffer.length) {
            self.buffer = self.buffer.slice(self.offset)
        }
    }
    if (self.length !== -1) {
        if (self.length <= self.buffer.length) {
            self.buffer = self.buffer.slice(0,self.length)
        }
    }
    for (var i = 0; i!=self.indent; ++i) {
        self.prefix = " "+self.prefix
    }
    var pos = 0
    this.toString = function () {
        var str = "<style> * { font-family: '"+ self.font + "'}</style>"
        if (self.html) { str += "<table border='1'>\n"}
        //split up into line of max `self.width`
        var line_arr = lines()
        //lines().forEach(function(hex_raw, i)
        for (var i = 0; i!= line_arr.length; ++i) {
            var hex_raw = line_arr[i],
                    hex = hex_raw[0],
                    raw = hex_raw[1]
            //insert spaces every `self.format.twos` or fours
            var howMany = hex.length
            if (self.format === "fours") {
                howMany = 4
            } else if (self.format === "twos") {
                howMany = 2
            }
            var hex_formatted = ""
            for (var j =0; j< hex.length; j+=howMany) {
                var s = hex.substr(j, howMany)
                hex_formatted += s + " "
            }
            var addr = (i*self.width)+self.offset+self.display_offset;
            if (self.html) {
                var odd = i%2 == 0 ? " even" : " odd"
                str += "<tr class='"+pad(addr, 8)+odd+"'>"
            }
            str += self.prefix
            str += "<td class='indexes'>"
            if (self.numbering === "hex_bytes") {
                str += pad(addr, 8) // padding...
                str += ": "
            }
            str += "</td><td>"
            var padlen = 0
            switch(self.format) {
            case "eights":
                padlen = self.width*2 + Math.floor(self.width/4)
                break
            case "fours":
                padlen = self.width*2 + Math.floor(self.width/2)
                break
            case "twos":
                padlen = self.width*3 + 2
                break
            default:
                padlen = self.width * 2 + 1
            }
            str += rpad(hex_formatted, padlen)
            str += "</td><td>"
            if (self.annotate === "ascii") {
                str+=" "
                str += escape(raw.replace(/[\000-\040\177-\377]/g, "."))
            }
            if (self.html) {
                str += "</td></tr>\n"
            } else {
                str += "\n"
            }
        }
        if (self.html) { str += "</table>\n"}
        return str
    }
    var lines = function() {
        var hex_raw = []
        for (var i = 0; i<self.buffer.length ; i+=self.width) {
            var begin = i,
                    end = i+self.width >= self.buffer.length ? self.buffer.length : i+self.width,
                                                               slice = self.buffer.slice(begin, end),
                                                               hex = self.caps === "upper" ? hexu(slice) : hexl(slice),
                                                                                             raw = String.fromCharCode.apply(null, slice)
            hex_raw.push([hex,raw])
        }
        return hex_raw
    }
    var hexl = function (buffer) {
        var str = ""
        for (var i=0; i!=buffer.length; ++i) {
            if (buffer.constructor == String) {
                str += pad(buffer.charCodeAt(i), 2)
            } else {
                str += pad(buffer[i], 2)
            }
        }
        return str
    }
    var hexu = function (buffer) {
        return hexl(buffer).toUpperCase()
    }
    var pad = function(b, len) {
        var s = b.toString(16)
        while (s.length < len) {
            s = "0" + s
        }
        return s
    }
    var rpad = function(s, len) {
        for (var n = len - s.length; n>0; --n) {
            if (self.html) {
                s += "&nbsp;"
            } else {
                s += " "
            }
        }
        return s
    }
    var escape = function (str) {
        str = str.split("&").join("&amp;")
        str = str.split("<").join("&lt;")
        str = str.split(">").join("&gt;")
        return str
    }
}
