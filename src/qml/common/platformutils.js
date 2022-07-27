
var dateTimeFormat = "yyyy-MM-dd hh:mm:ss.zzz"

function isScalingDisabled() {
    return (Qt.platform.os == "windows" || Qt.platform.os == "linux")
            && screen.devicePixelRatio < 2
}

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

function getThemeIcon(icon) {
    if (sysPalette.base.hslLightness < 0.4) {
        return "qrc:/images/dark_theme/" + icon
    } else {
        return "qrc:/images/light_theme/" + icon
    }
}
