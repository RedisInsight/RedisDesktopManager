.import "./../../3rdparty/php-unserialize-js/phpUnserialize.js" as Parser

console.log("---> parser: ", Parser.qt_phpUnserialize)

function unserialize(data) {
    return Parser.qt_phpUnserialize(data)
}
