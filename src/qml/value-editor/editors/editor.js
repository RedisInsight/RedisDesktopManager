
function getSupportedKeyTypes() {
    return ["string", "list", "set", "zset", "hash"]
}

function getEditorByTypeString(keyType) {
    if (keyType === "string") {
        return "./editors/SingleItemEditor.qml"
    } else if (keyType === "list" || keyType === "set") {
        return "./editors/SingleItemEditor.qml"
    } else if (keyType === "zset") {
        return "./editors/SortedSetItemEditor.qml"
    } else if (keyType === "hash") {
        return "./editors/HashItemEditor.qml"
    } else if (keyType === "ReJSON") {
        return "./editors/SingleItemEditor.qml"
    } else {
        console.error("Editor for type " + keyType + " is not defined!")
    }
}

