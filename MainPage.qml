import QtQuick.Controls
import QtQuick.Layouts
import QtQuick
import QtCore
import QtQuick.Dialogs
import CusModel
//主页面
Item {
    property alias contactsListView: contactsListView
    property alias sessionListView: sessionListView
    FileDialog {
        id: imgDialog
        title: qsTr("选择图片")
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        nameFilters: ["image (*.jpg; *.png)"]
        onAccepted: {
            var filepath = selectedFile.toString()
            if (filepath.length > 8) {
                var imageName = filepath.substring(8)
                console.log(imageName)
                if (comm.updateHeadImage(imageName))
                    headImg.source = imageName
            }
        }
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        RowLayout {
            Layout.fillWidth: true
            //头像
            CircleHead {
                id: headImg
                width: 50
                height: 50
                MouseArea {
                    anchors.fill: parent
                    onClicked: imgDialog.open()
                }
            }
            TextField {
                id: headName
                font.family: 'Microsoft YaHei UI'
                color: 'gray'
                selectByMouse: true
                background: Rectangle {
                    id: textRect
                    color: 'transparent'
                    border.color: parent.activeFocus ? '#3e4452' : 'transparent'
                }
                onEditingFinished: {
                    swipeView.forceActiveFocus()
                    comm.updateName(headName.text)
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Item {
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("会话")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked :swipeView.currentIndex = 0
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("联系人")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked :swipeView.currentIndex = 1
            }
            Item {
                Layout.fillWidth: true
            }
        }
        //列表
        SwipeView {
            id: swipeView
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            //会话列表
            SessionListView {
                id: sessionListView
            }
            //联系人列表
            ContactsListView {
                id: contactsListView
            }
        }
    }
    Component.onCompleted: {
        var info = comm.login()
        headImg.source = info.image.toString()
        headName.text = info.name.toString()
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        propagateComposedEvents: true
        onPressed: {
            mouse.accepted = false
            if (!textRect.contains(Qt.point(mouseX, mouseY)))
                swipeView.forceActiveFocus()
        }
    }
}
