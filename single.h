#ifndef SINGLE_H
#define SINGLE_H
#include <vector>
#include <QObject>
struct UserInfo
{
    QString id;
    QString addr;
    QString name;
    QString image;
    QString message;
};
enum ChatRoles
{
    Name = Qt::UserRole + 1,
    Image,
    Message
};
class Single
{
public:
    static std::vector<UserInfo> contacts, sessions;
    static std::vector<int> test;
    static QString id;
};
#endif
