#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H
#include <QAbstractListModel>
#include <QHostAddress>
#include "single.h"
class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
public slots:
    void onNameInfo(const UserInfo&);
    void onImageInfo(const QString&, const QString&);
    void onMsgInfo(const QString&, const QString&);
    void onLogOut(const QString&);
    void onClearModel();
public:
    enum ChatRoles
    {
        Name = Qt::UserRole + 1,
        Image,
        Message
    };
    explicit ChatListModel(bool, QObject *parent = nullptr);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QVariantMap getData(int) const;
    Q_INVOKABLE int findSession(const QString&, const QString&, const QString&, const QString&);
private:
    void updateNameInfo(const UserInfo&, bool);
    void updateImageInfo(const QString&, const QString&, bool);
    bool isContacts;
signals:
    void newMessage(bool, const QVariantMap&);
};
#endif
