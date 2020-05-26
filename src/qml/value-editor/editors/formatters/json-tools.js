.pragma library

/*
    Based on:
    json-format v.1.1
    http://github.com/phoboslab/json-format
    Released under MIT license:
    http://www.opensource.org/licenses/mit-license.php
*/

var prettyPrint = function( json, style, colorMap) {
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

    return '<pre id="value" style="width:97%; white-space: pre-wrap;' + style + '">' + highlighted + '</pre>';
};

/*!
    Based on:

    JSON.minify()
    v0.1 (c) Kyle Simpson
    MIT License

    https://github.com/getify/JSON.minify/blob/javascript/minify.json.js
*/
var minify = function(json) {

    var tokenizer = /"|(\/\*)|(\*\/)|(\/\/)|\n|\r/g,
    in_string = false,
    in_multiline_comment = false,
    in_singleline_comment = false,
    tmp, tmp2, new_str = [], ns = 0, from = 0, lc, rc;

    tokenizer.lastIndex = 0;

    json = json.concat("\n");

    while (tmp = tokenizer.exec(json)) {
        lc = RegExp.leftContext;
        rc = RegExp.rightContext;
        if (!in_multiline_comment && !in_singleline_comment) {
            tmp2 = lc.substring(from);
            if (!in_string) {
                tmp2 = tmp2.replace(/(\n|\r|\s)*/g,"");
            }
            new_str[ns++] = tmp2;
        }
        from = tokenizer.lastIndex;

        if (tmp[0] == "\"" && !in_multiline_comment && !in_singleline_comment) {
            tmp2 = lc.match(/(\\)*$/);
            if (!in_string || !tmp2 || (tmp2[0].length % 2) == 0) {	// start of string with ", or unescaped " character found to end string
                in_string = !in_string;
            }
            from--; // include " character in next catch
            rc = json.substring(from);
        }
        else if (tmp[0] == "/*" && !in_string && !in_multiline_comment && !in_singleline_comment) {
            in_multiline_comment = true;
        }
        else if (tmp[0] == "*/" && !in_string && in_multiline_comment && !in_singleline_comment) {
            in_multiline_comment = false;
        }
        else if (tmp[0] == "//" && !in_string && !in_multiline_comment && !in_singleline_comment) {
            in_singleline_comment = true;
        }
        else if ((tmp[0] == "\n" || tmp[0] == "\r") && !in_string && !in_multiline_comment && in_singleline_comment) {
            in_singleline_comment = false;
        }
        else if (!in_multiline_comment && !in_singleline_comment && !(/\n|\r|\s/.test(tmp[0]))) {
            new_str[ns++] = tmp[0];
        }
    }
    new_str[ns++] = rc;
    return new_str.join("");
};

if (typeof WorkerScript !== "undefined") {
    WorkerScript.onMessage = function(msg) {
        WorkerScript.sendMessage({
            'error': msg['error'],
             // NOTE(u_glide): Minify json before processing to get rid of double formatted JSON
            'formatted': prettyPrint(minify(String(msg['data'])), msg['style'], msg['color_map']),
            'isReadOnly': msg['isReadOnly'],
            'format': "html"
        });
    }
}
