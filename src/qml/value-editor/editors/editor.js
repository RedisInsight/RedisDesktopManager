
function getSupportedKeyTypes() {
    return ["string", "list", "set", "zset", "hash", "stream"]
}

function getEditorByTypeString(keyType) {
    if (keyType === "string"
            || keyType === "hyperloglog"
            || keyType === "list"
            || keyType === "set"
            || keyType === "ReJSON") {
        return "./editors/SingleItemEditor.qml"    
    } else if (keyType === "zset") {
        return "./editors/SortedSetItemEditor.qml"
    } else if (keyType === "hash") {
        return "./editors/HashItemEditor.qml"
    } else if (keyType === "stream") {
        return "./editors/StreamItemEditor.qml"
    } else if (keyType) {
        console.error("Editor for type " + keyType + " is not defined!")
    }
}

