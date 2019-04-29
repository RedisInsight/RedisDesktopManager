/*
    Based on:
    json-format v.1.1
    http://github.com/phoboslab/json-format
    Released under MIT license:
    http://www.opensource.org/licenses/mit-license.php
*/

var prettyPrint = function( json ) {
    var p = []
    var out = ""
    var indent = 0;

    var push = function( m ) { return '\\' + p.push( m ) + '\\'; }
    var pop = function( m, i ) { return p[i-1] }
    var tabs = function( count ) { return new Array( count + 1 ).join( '  ' ); }

    // Extract backslashes and strings
    json = json
        .replace( /\\./g, push )
        .replace( /(".*?"|'.*?')/g, push )
        .replace( /\s+/, '' );

    // Indent and insert newlines
    for( var i = 0; i < json.length; i++ ) {
        var c = json.charAt(i);

        switch(c) {
            case '{':
            case '[':
                out += c + "\n" + tabs(++indent);
                break;
            case '}':
            case ']':
                out += "\n" + tabs(--indent) + c;
                break;
            case ',':
                out += ",\n" + tabs(indent);
                break;
            case ':':
                out += ": ";
                break;
            default:
                out += c;
                break;
        }
    }

    // Strip whitespace from numeric arrays and put backslashes
    // and strings back in
    out = out
        .replace( /\[[\d,\s]+?\]/g, function(m){ return m.replace(/\s/g,''); } )
        .replace( /\\(\d+)\\/g, pop ) // strings
        .replace( /\\(\d+)\\/g, pop ); // backslashes in strings

    var colorMap = {
            string: '#008000',
            number: '#0000ff',
            boolean: '#b22222',
            null: '#808080',
            key: '#000000'
        };

    // Highlight different value types
    var syntaxHighlight = function(json) {
        if (typeof json != 'string') {
            json = JSON.stringify(json, undefined, 2);
        }

        json = json.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');

        return json.replace(/("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?)/g, function (match) {
            var type = 'number';
            if (/^"/.test(match)) {
                if (/:$/.test(match)) {
                    type = 'key';
                } else {
                    type = 'string';
                }
            } else if (/true|false/.test(match)) {
                type = 'boolean';
            } else if (/null/.test(match)) {
                type = 'null';
            }
            return '<font color="' + colorMap[type] + '">' + match + '</font>';
        });
    }

    var highlighted = syntaxHighlight(out);

//    return out;
    return '<pre id="value">' + highlighted + '</pre>';
};


/*!
    Based on:

    * pretty-data - nodejs plugin to pretty-print or minify data in XML, JSON and CSS formats.
    *
    * Version - 0.40.0
    * Copyright (c) 2012 Vadim Kiryukhin
    * vkiryukhin @ gmail.com
    * http://www.eslinstructor.net/pretty-data/
    * Dual licensed under the MIT and GPL licenses
*/
var minify =  function(json) {

    return  json.replace(/\s{0,}\{\s{0,}/g,"{")
                .replace(/\s{0,}\[$/g,"[")
                .replace(/\[\s{0,}/g,"[")
                .replace(/:\s{0,}\[/g,':[')
                .replace(/\s{0,}\}\s{0,}/g,"}")
                .replace(/\s{0,}\]\s{0,}/g,"]")
                .replace(/\"\s{0,}\,/g,'",')
                .replace(/\,\s{0,}\"/g,',"')
                .replace(/\"\s{0,}:/g,'":')
                .replace(/:\s{0,}\"/g,':"')
                .replace(/:\s{0,}\[/g,':[')
                .replace(/\,\s{0,}\[/g,',[')
                .replace(/\,\s{2,}/g,', ')
                .replace(/\]\s{0,},\s{0,}\[/g,'],[');
};

