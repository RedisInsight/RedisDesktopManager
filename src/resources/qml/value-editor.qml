import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import "."

Item {
    anchors.fill: parent

    implicitHeight: 600

    TabView {
        anchors.fill: parent
        currentIndex: 0

        WelcomeTab {}

        ValueTabs { model: viewModel }
    }
}
