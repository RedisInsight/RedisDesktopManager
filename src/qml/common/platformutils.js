
var dateTimeFormat = "yyyy-MM-dd hh:mm:ss.zzz"

function isWindows() {
    return Qt.platform.os == "windows"
}

function isLinux() {
    return Qt.platform.os == "linux"
}

function isOSX() {
    return Qt.platform.os == "osx"
}

function isOSXRetina(screen) {
    return isOSX() && screen.devicePixelRatio> 1
}

