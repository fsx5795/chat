#ifndef COMM_H
#define COMM_H
#include <QHostAddress>
#include <QUdpSocket>
#include <QSettings>
#include "single.h"
#pragma pack(1)
/*!
 @brief 用户信息相关类
*/
class Comm : public QObject
{
    Q_OBJECT
    enum class ChatType
    {
        Login,
        Image,
        Name,
        Message,
        Logout
    };
    struct HeadInfo
    {
        //头部标识0xaa
        uint8_t start;
        ChatType type;
        char id[35];
        HeadInfo() : start(0xaa)
        {
            memset(this->id, 0, sizeof(this->id));
        }
    };
    struct NameInfo
    {
        HeadInfo headInfo;
        char name[10];
        //尾部标识0xee
        uint8_t end;
        NameInfo() : end(0xee)
        {
            this->headInfo.type = ChatType::Name;
            memset(this->name, 0, sizeof(this->name));
        }
    };
    struct ImageInfo
    {
        HeadInfo headInfo;
        //头像图片名
        char img[50];
        //头像数据总MD5值
        char imgMD5[50];
        //尾部标识0xee
        uint8_t end;
        ImageInfo() : end(0xee)
        {
            this->headInfo.type = ChatType::Image;
            memset(this->img, 0, sizeof(this->img));
            memset(this->imgMD5, 0, sizeof(this->imgMD5));
        }
    };
    struct MessageInfo
    {
        HeadInfo headInfo;
        char msg[50];
        uint8_t end;
        MessageInfo() : end(0xee)
        {
            this->headInfo.type = ChatType::Message;
            memset(this->msg, 0, sizeof(this->msg));
        }
    };
public:
    explicit Comm(QObject *parent = nullptr);
    Q_INVOKABLE QVariantMap login();
    Q_INVOKABLE bool updateHeadImage(const QString&);
    Q_INVOKABLE void updateName(const QString&);
    Q_INVOKABLE bool sendMessage(const QString&, const QString&);
    virtual ~Comm() override;
private slots:
    void onReadyRead();
private:
    UserInfo getAdminInfo();
    QByteArray getImageData(const QString&);
    void sendName(const QHostAddress&, const QString&);
    void sendImage(const QHostAddress&, const QString&, const QByteArray&);
    //向指定地址发送本机用户信息
    void sendInfo(const QHostAddress&);
    QUdpSocket *udp;
    const QHostAddress multicast;
    const quint16 port;
    QSettings settings;
    QString prevName;
signals:
    void nameInfo(const UserInfo&);
    void imageInfo(const QString&, const QString&);
    void msgInfo(const QString&, const QString&);
    void errorInfo(const QString&);
    void logOut(const QString&);
    void clearModel();
};
#pragma pack()
#endif