import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import com.fortfirewall 1.0

ColumnLayout {

    property int index
    property AppGroup appGroup

    RowLayout {
        Button {
            icon.source: "qrc:/images/application_delete.png"
            text: translationManager.dummyBool
                  && qsTranslate("qml", "Remove Group")
            onClicked: removeAppGroup(index)
        }
        Button {
            icon.source: "qrc:/images/resultset_previous.png"
            text: translationManager.dummyBool
                  && qsTranslate("qml", "Move left")
            onClicked: moveAppGroup(index, -1)
        }
        Button {
            icon.source: "qrc:/images/resultset_next.png"
            text: translationManager.dummyBool
                  && qsTranslate("qml", "Move right")
            onClicked: moveAppGroup(index, 1)
        }

        SpeedLimitButton {}

        Item {
            Layout.fillWidth: true
        }

        CheckBox {
            text: translationManager.dummyBool
                  && qsTranslate("qml", "Enabled")
            checked: appGroup.enabled
            onToggled: {
                appGroup.enabled = checked;

                setConfFlagsEdited();
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 10

        AppsTextColumn {
            title {
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Block")
            }

            textArea {
                placeholderText: "
C:\\Program Files\\Internet Explorer\\iexplore.exe
"
                text: appGroup.blockText
            }

            onTextChanged: {
                if (appGroup.blockText == textArea.text)
                    return;

                appGroup.blockText = textArea.text;

                setConfEdited();
            }
        }

        AppsTextColumn {
            title {
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Allow")
            }

            textArea {
                placeholderText: "
System
C:\\Program Files\\Skype\\Phone\\Skype.exe
"
                text: appGroup.allowText
            }

            onTextChanged: {
                if (appGroup.allowText == textArea.text)
                    return;

                appGroup.allowText = textArea.text;

                setConfEdited();
            }
        }
    }
}
