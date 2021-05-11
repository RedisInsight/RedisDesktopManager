
var dateTimeFormat = "yyyy-MM-dd hh:mm:ss.zzz"

function isOSX() {
    return Qt.platform.os == "osx"
}

function isOSXRetina(screen) {
    return isOSX() && screen.devicePixelRatio> 1
}

