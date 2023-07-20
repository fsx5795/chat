#ifndef SINGLETONTYPEEXAMPLE_H
#define SINGLETONTYPEEXAMPLE_H
#include "comm.h"
/*
#include "contactsmodel.h"
#include "sessionmodel.h"
*/
#include "chatlistmodel.h"
class SingletonTypeExample : public QObject
{
    Q_OBJECT
    /*
    Q_PROPERTY(ContactsModel* contactsModel)
    Q_PROPERTY(SessionModel* sessionModel)
    */
    Q_PROPERTY(ChatListModel* contactsModel)
    Q_PROPERTY(ChatListModel* sessionModel)
    Q_PROPERTY(Comm* comm)
public:
    explicit SingletonTypeExample(QObject *parent = nullptr);
    Q_INVOKABLE Comm* getComm();
    /*
    Q_INVOKABLE ContactsModel* getContactsModel();
    Q_INVOKABLE SessionModel* getSessionModel();
    */
    Q_INVOKABLE ChatListModel* getContactsModel();
    Q_INVOKABLE ChatListModel* getSessionModel();
private:
    Comm *comm;
    /*
    ContactsModel *contactsModel;
    SessionModel *sessionModel;
    */
    ChatListModel *contactsModel;
    ChatListModel *sessionModel;
};
#endif