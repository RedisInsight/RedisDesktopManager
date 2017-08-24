import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1

Tab {
    property string icon
    property bool closable: true
    property string tabType

    signal close(int index)

    function closeTab(index) {
        close(index)
    }
}
