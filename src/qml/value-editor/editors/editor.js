
function getSupportedKeyTypes() {
    return ["string", "list", "set", "zset", "hash"]
}

function getEditorByTypeString(keyType) {
    if (keyType === "string") {
        return "./value-editor/editors/SingleItemEditor.qml"
    } else if (keyType === "list" || keyType === "set") {
        return "./value-editor/editors/SingleItemEditor.qml"
    } else if (keyType === "zset") {
        return "./value-editor/editors/SortedSetItemEditor.qml"
    } else if (keyType === "hash") {
        return "./value-editor/editors/HashItemEditor.qml"
    } else {
        console.error("Editor for type " + keyType + " is not defined!")
    }
}

