import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

SpinBox {
    implicitHeight: 30
    editable: true
    textFromValue: function(value, locale) { return value }
}
