.import "./../../../3rdparty/php-unserialize-js/phpUnserialize.js" as Parser

function unserialize(data) {
    return Parser.qt_phpUnserialize(data)
}
