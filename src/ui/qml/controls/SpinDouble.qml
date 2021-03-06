import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

RowLayout {

    readonly property alias field1: field1
    readonly property alias field2: field2

    property real fieldPreferredWidth

    SpinBoxControl {
        id: field1
        Layout.fillWidth: true
        Layout.preferredWidth: fieldPreferredWidth
        Layout.minimumWidth: fieldPreferredWidth
    }

    SpinBoxControl {
        id: field2
        Layout.fillWidth: true
        Layout.preferredWidth: fieldPreferredWidth
        Layout.minimumWidth: fieldPreferredWidth
    }
}
