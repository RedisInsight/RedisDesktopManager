
function isOSX() {
    return Qt.platform.os == "osx"
}

function isOSXRetina(screen) {
    return isOSX() && screen.devicePixelRatio> 1
}

function monospacedFontFamily() {
    var platform = Qt.platform.os;

    if (platform === "osx")
        return "Monaco";

    if (platform === "windows")
        return "Segoe UI Mono"

    return "Ubuntu Mono"
}

