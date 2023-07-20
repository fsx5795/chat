import QtQuick.Controls
import QtQml
import QtQuick.Layouts
import QtQuick
import CusModel
//中心窗口
SwipeView {
    id: mainSwipe
    anchors.fill: parent
    interactive: false
    signal newMessage(var varMap)
    //主页面
    MainPage{
        id: mainPage
    }
    Component {
        id: stackPage
        Item {
            id: stackItem
            property string curId
            property string img
            property string name
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: qsTr("返回")
                        onClicked: mainSwipe.currentIndex = 0
                    }
                    CircleHead {
                        width: 50
                        height: 50
                        source: stackItem.img
                    }
                    Text {
                        font.family: 'Microsoft YaHei UI'
                        text: stackItem.name
                        color: 'gray'
                    }
                }
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ListView {
                        id: messageListView
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    TextField {
                        id: sendTextField
                        Layout.fillWidth: true
                    }
                    Button {
                        implicitWidth: 100
                        implicitHeight: 30
                        text: qsTr("发送")
                        onClicked: {
                            if (sendTextField.text.length != 0) {
                                if (comm.sendMessage(stackItem.curId, sendTextField.text))
                                    sendTextField.text = ""
                            }
                        }
                    }
                }
            }
        }
    }
    //会话详情页面
    StackLayout {
        id: stackLayout
    }
    Connections {
        target: mainPage.contactsListView
        //联系人点击槽函数
        function onListItemClicked(cId, curAddr, curName, curImage) {
            var stackCount = stackLayout.count
            //查找点击的联系人在会话列表详情中的哪个位置
            var curIndex = mainPage.sessionListView.model.findSession(cId, curAddr, curName, curImage)
            if (curIndex >= stackCount)
                stackPage.createObject(stackLayout, {curId: cId, addr: curAddr, img: curImage, name: curName})
            stackLayout.currentIndex = curIndex
            mainSwipe.currentIndex = 1
        }
    }
    Connections {
        target: mainPage.sessionListView
        function onListItemClicked(idx, addr, name, img) {
            stackLayout.currentIndex = idx
            mainSwipe.currentIndex = 1
        }
    }
    Connections {
        target: mainPage.sessionListView.model
        function onNewMessage(isNew, varMap) {
            if (isNew)
                stackPage.createObject(stackLayout, {curId: varMap.id, img: varMap.image, name: varMap.name})
            newMessage(varMap)
        }
    }
}