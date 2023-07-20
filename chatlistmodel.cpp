#include "chatlistmodel.h"
ChatListModel::ChatListModel(bool isContacts, QObject *parent) : isContacts(isContacts), QAbstractListModel(parent){}
int ChatListModel::rowCount(const QModelIndex &parent) const
{
    return this->isContacts ? Single::contacts.size() : Single::sessions.size();
}
QVariant ChatListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (!index.isValid() || index.row() >= this->rowCount() || index.row() < 0)
        return QVariant();
    switch (role)
    {
    case ChatRoles::Name:
        return this->isContacts ? Single::contacts.at(index.row()).name : Single::sessions.at(index.row()).name;
    case ChatRoles::Image:
        return this->isContacts ? Single::contacts.at(index.row()).image : Single::sessions.at(index.row()).image;
    case ChatRoles::Message:
        return this->isContacts ? Single::contacts.at(index.row()).message : Single::sessions.at(index.row()).message;
    default:
        break;
    }
    return QVariant();
}
QHash<int, QByteArray> ChatListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ChatRoles::Name] = "name";
    roles[ChatRoles::Image] = "image";
    roles[ChatRoles::Message] = "message";
    return roles;
}
Q_INVOKABLE QVariantMap ChatListModel::getData(int index) const
{
    QVariantMap varMap;
    if (index < 0 || index >= this->rowCount())
        return varMap;
    if (this->isContacts)
    {
        varMap.insert("id", Single::contacts.at(index).id);
        varMap.insert("addr", Single::contacts.at(index).addr);
        varMap.insert("name", Single::contacts.at(index).name);
        varMap.insert("image", Single::contacts.at(index).image);
    }
    else
    {
        varMap.insert("id", Single::sessions.at(index).id);
        varMap.insert("addr", Single::sessions.at(index).addr);
        varMap.insert("name", Single::sessions.at(index).name);
        varMap.insert("image", Single::sessions.at(index).image);
        varMap.insert("message", Single::sessions.at(index).message);
    }
    return varMap;
}
void ChatListModel::updateNameInfo(const UserInfo &userInfo, bool isCon)
{
    std::vector<UserInfo> &datas = isCon ? Single::contacts : Single::sessions;
    std::vector<UserInfo>::iterator it = datas.begin();
    while (it != datas.end())
    {
        if (it->id.compare(userInfo.id) == 0)
        {
            this->beginResetModel();
            it->addr = userInfo.addr;
            it->name = userInfo.name;
            this->endResetModel();
            break;
        }
        ++it;
    }
    //如果当前所需更新的是联系人列表且联系人列表中没有该联系人
    if (it == datas.end() && isCon)
    {
        this->beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
        datas.push_back({userInfo.id, userInfo.addr, userInfo.name}); 
        this->endInsertRows();
    }
    return;
}
void ChatListModel::updateImageInfo(const QString &id, const QString &imgPath, bool isCon)
{
    std::vector<UserInfo> &datas = isCon ? Single::contacts : Single::sessions;
    std::vector<UserInfo>::iterator it = datas.begin();
    while (it != datas.end())
    {
        if (it->id.compare(id) == 0)
        {
            this->beginResetModel();
            it->image = imgPath;
            this->endResetModel();
            break;
        }
        ++it;
    }
    return;
}
Q_INVOKABLE int ChatListModel::findSession(const QString &id, const QString &addr, const QString &name, const QString &img)
{
    for (int index{0}; index < Single::sessions.size(); ++index)
    {
        if (Single::sessions.at(index).id.compare(id) == 0)
            return index;
    }
    this->beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());
    Single::sessions.push_back({id, addr, name, img});
    this->endInsertRows();
    return Single::sessions.size() - 1;
}
void ChatListModel::onNameInfo(const UserInfo &userInfo)
{
    this->updateNameInfo(userInfo, true);
    this->updateNameInfo(userInfo, false);
    return;
}
void ChatListModel::onImageInfo(const QString &id, const QString &imgPath)
{
    this->updateImageInfo(id, imgPath, true);
    this->updateImageInfo(id, imgPath, false);
    return;
}
void ChatListModel::onMsgInfo(const QString &id, const QString &message)
{
    unsigned int index{ 0 };
    std::vector<UserInfo>::iterator it = Single::sessions.begin();
    while (it != Single::sessions.end())
    {
        if (it->id.compare(id) == 0)
        {
            this->beginResetModel();
            it->message = message;
            this->endResetModel();
            break;
        }
        ++it;
        ++index;
    }
    for (UserInfo datas : Single::contacts)
    {
        if (datas.id.compare(id) == 0)
        {
            bool isNew{it == Single::sessions.end()};
            if (isNew)
            {
                this->beginInsertRows(QModelIndex(), Single::sessions.size(), Single::sessions.size());
                Single::sessions.push_back({id, datas.addr, datas.name, datas.image, message});
                this->endInsertRows();
            }
            QVariantMap varMap;
            varMap.insert("id", id);
            varMap.insert("name", datas.name);
            varMap.insert("image", datas.image);
            varMap.insert("message", message);
            emit this->newMessage(isNew, varMap);
            break;
        }
    }
    return;
}
void ChatListModel::onLogOut(const QString &id)
{
    unsigned int index{ 0 };
    std::vector<UserInfo>::iterator it = Single::contacts.begin();
    while (it != Single::contacts.end())
    {
        if (it->id.compare(id) == 0)
        {
            this->beginRemoveRows(QModelIndex(), index, index);
            Single::contacts.erase(it);
            this->endRemoveRows();
            break;
        }
        ++it;
        ++index;
    }
    return;
}
void ChatListModel::onClearModel()
{
    this->beginRemoveRows(QModelIndex(), 0, Single::contacts.size());
    Single::contacts.clear();
    this->endRemoveRows();
    return;
}
