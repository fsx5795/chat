import QtQuick.Controls
import Qt.labs.platform as Pf
import QtQml
import QtQuick
import Qt.example.qobjectSingleton
import QtQuick.Window
ApplicationWindow {
    id: mainWindow
    property real shellLength: 4
    //获取用户信息相关类
    property var comm: Example.getComm()
    visible: true
    width: 345
    height: 650
    //中心窗口
    ContentView {
        id: contentView
    }
    Pf.SystemTrayIcon {
        id: tray
        visible: Qt.platform.os === "windows"
        icon.source: 'qrc:/image.png'
        tooltip: qsTr('会社通')
        menu: Pf.Menu {
            Pf.MenuItem {
                text: qsTr("退出")
                onTriggered: Qt.quit()
            }
        }
    }
    Connections {
        target: tray
        function onActivated(reason) {
            if (reason === Pf.SystemTrayIcon.Trigger) {
                mainWindow.show()
                mainWindow.raise()
                mainWindow.requestActivate()
            }
        }
        function onMessageClicked() {
            mainWindow.show()
            mainWindow.raise()
            mainWindow.requestActivate()
        }
    }
    Connections {
        target: comm
        //用户信息相关错误
        function onErrorInfo(str) {
            tray.showMessage(qsTr("错误"), str)
        }
    }
    Connections {
        target: contentView
        function onNewMessage(varMap) {
            if (mainWindow.visibility === Window.Minimized || mainWindow.visibility === Window.Hidden)
                tray.showMessage(varMap.name, varMap.message)
        }
    }
}
