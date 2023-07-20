import QtQuick 2.15
import QtQuick.Layouts 1.15
import Qt.example.qobjectSingleton 1.0
import CusModel 1.0
ListView {
    id: listView
    signal listItemClicked(string addr, string img, string name, string image)
    model: Example.getContactsModel() 
    delegate: Item {
        height: 50
        implicitWidth: listView.width
        RowLayout {
            CircleHead {
                width: 40
                height: 40
                source: model.image
            }
            Text {
                font.family: 'Microsoft YaHei UI'
                text: model.name
                color: 'gray'
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                let data = listView.model.getData(index)
                listView.listItemClicked(data.id, data.addr, data.name, data.image)
            }
        }
    }
}
