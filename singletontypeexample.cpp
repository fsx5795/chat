#include "singletontypeexample.h"
SingletonTypeExample::SingletonTypeExample(QObject *parent)
    //: comm(new Comm()), contactsModel(new ContactsModel()), sessionModel(new SessionModel()), QObject(parent)
    : comm(new Comm()), contactsModel(new ChatListModel(true)), sessionModel(new ChatListModel(false)), QObject(parent)
{
    /*
    connect(this->comm, &Comm::userInfo, this->contactsModel, &ContactsModel::onUserInfo);
    connect(this->comm, &Comm::msgInfo, this->sessionModel, &SessionModel::onMsgInfo);
    connect(this->comm, &Comm::logOut, this->contactsModel, &ContactsModel::onLogOut);
    connect(this->comm, &Comm::clearModel, this->contactsModel, &ContactsModel::onClearModel);
    */
    connect(this->comm, &Comm::nameInfo, this->contactsModel, &ChatListModel::onNameInfo);
    connect(this->comm, &Comm::imageInfo, this->contactsModel, &ChatListModel::onImageInfo);
    connect(this->comm, &Comm::msgInfo, this->sessionModel, &ChatListModel::onMsgInfo);
    connect(this->comm, &Comm::logOut, this->contactsModel, &ChatListModel::onLogOut);
    connect(this->comm, &Comm::clearModel, this->contactsModel, &ChatListModel::onClearModel);
}
Q_INVOKABLE Comm* SingletonTypeExample::getComm()
{
    return this->comm;
}
//Q_INVOKABLE ContactsModel* SingletonTypeExample::getContactsModel()
Q_INVOKABLE ChatListModel* SingletonTypeExample::getContactsModel()
{
    return this->contactsModel;
}
//Q_INVOKABLE SessionModel* SingletonTypeExample::getSessionModel()
Q_INVOKABLE ChatListModel* SingletonTypeExample::getSessionModel()
{
    return this->sessionModel;
}