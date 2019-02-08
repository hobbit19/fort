import QtQuick 2.12
import QtQuick.Controls 2.12

TextField {
    id: textField
    persistentSelection: true
    selectByMouse: true
    onReleased: textContextMenu.show(event, textField)
}
