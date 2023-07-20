#include <QCoreApplication>
#include <QUuid>
#include <QFile>
#include <QFileInfo>
#include <QVariantMap>
#include <QImage>
#include <QBuffer>
#include <QCryptographicHash>
#include <QNetworkInterface>
#include "comm.h"
Comm::Comm(QObject *parent)
    : udp(new QUdpSocket(this))
    , settings(QCoreApplication::applicationDirPath().append("/userinfo.ini"), QSettings::IniFormat)
    , multicast("239.12.23.34")
    , port(9527)
    , QObject(parent)
{
    this->udp->bind(QHostAddress::AnyIPv4, this->port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    this->udp->joinMulticastGroup(this->multicast);
    this->udp->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
    connect(this->udp, &QUdpSocket::readyRead, this, &Comm::onReadyRead);
}
/*!
 @brief 获取当前本地用户信息
 @return 当前本地用户信息
*/
UserInfo Comm::getAdminInfo()
{
    UserInfo info;
    this->settings.beginGroup("info");
    Single::id = this->settings.value("id").toString();
    //如果当前没有本地用户信息
    if (Single::id.isEmpty()) {
        Single::id = QUuid::createUuid().toString().remove('{').remove('-').remove('}');
        this->settings.setValue("id", Single::id);
        //以本机IP地址作用初始用户名
        QList<QHostAddress> list = QNetworkInterface::allAddresses();
        for (QHostAddress addr : list) {
            QString str = addr.toString();
            if (str.count() > 10 && str.count() <= 16) {
                qDebug() << addr.toString();
                this->settings.setValue("name", addr.toString());
                break;
            }
        }
    }
    info.name = QString::fromUtf8(this->settings.value("name").toByteArray());
    info.image = this->settings.value("image").toString();
    this->settings.endGroup();
    return info;
}
QByteArray Comm::getImageData(const QString &imagePath)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    QImage image;
    image.load(imagePath);
    image.save(&buffer, QFileInfo(imagePath).suffix().toUtf8().data());
    imageData = imageData.toBase64();
    return imageData;
}
Q_INVOKABLE QVariantMap Comm::login() 
{
    QVariantMap varMap;
    UserInfo adminInfo = this->getAdminInfo();
    varMap.insert("name", adminInfo.name);
    varMap.insert("image", adminInfo.image);
    HeadInfo headInfo;
    headInfo.type = ChatType::Login;
    this->udp->writeDatagram(reinterpret_cast<char*>(&headInfo), sizeof(HeadInfo), this->multicast, this->port);
    this->sendName(this->multicast, adminInfo.name);
    this->sendImage(this->multicast, adminInfo.image, this->getImageData(adminInfo.image));
    return varMap;
}
Q_INVOKABLE void Comm::updateName(const QString &name)
{
    if (this->prevName.compare(name) == 0)
        return;
    this->prevName = name;
    this->settings.beginGroup("info");
    this->settings.setValue("name", name);
    this->settings.endGroup();
    this->sendName(this->multicast, name);
    return;
}
void Comm::sendName(const QHostAddress &addr, const QString &name)
{
    NameInfo info;
    strcpy(info.name, name.toUtf8().data());
    strcpy(info.headInfo.id, Single::id.toUtf8().data());
    this->udp->writeDatagram(reinterpret_cast<char*>(&info), sizeof(NameInfo), addr, this->port);
    return;
}
Q_INVOKABLE bool Comm::updateHeadImage(const QString &headImage)
{
    QByteArray imageData = this->getImageData(headImage);
    if (imageData.size() >= 65500)
    {
        emit this->errorInfo(QStringLiteral("图片过大，头像设置失败!"));
        return false;
    }
    QString targetImage = QCoreApplication::applicationDirPath().append("/").append(QFileInfo(headImage).fileName());
    if (QFile::exists(targetImage))
        QFile::remove(targetImage);
    QFile::copy(headImage, targetImage);
    this->settings.beginGroup("info");
    this->settings.setValue("image", targetImage);
    this->settings.endGroup();
    this->sendImage(this->multicast, headImage, imageData);
    return true;
}
void Comm::sendImage(const QHostAddress &addr, const QString &image, const QByteArray &data)
{
    ImageInfo info;
    strcpy(info.headInfo.id, Single::id.toUtf8().data());
    strcpy(info.img, QFileInfo(image).suffix().toUtf8().data());
    QByteArray md5 = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    QByteArray hex = md5.toHex();
    strcpy(info.imgMD5, hex.data());
    this->udp->writeDatagram(reinterpret_cast<char*>(&info), sizeof(ImageInfo), addr, this->port);
    this->udp->writeDatagram(data, addr, this->port);
    return;
}
Q_INVOKABLE bool Comm::sendMessage(const QString &id, const QString &message)
{
    MessageInfo msgInfo;
    strcpy(msgInfo.headInfo.id, Single::id.toUtf8().data());
    strcpy(msgInfo.msg, message.toUtf8().data());
    for (const UserInfo &info : Single::contacts)
    {
        if (info.id.compare(id) == 0)
            return this->udp->writeDatagram(reinterpret_cast<char*>(&msgInfo), sizeof(MessageInfo), QHostAddress(info.addr), this->port) >= 0;
    }
    return false;
}
/*!
 @brief 向指定地址发送本机用户信息
 @param addr 发送的地址
*/
void Comm::sendInfo(const QHostAddress &addr)
{
    UserInfo info = this->getAdminInfo();
    this->sendName(addr, info.name);
    this->sendImage(addr, info.image, this->getImageData(info.image));
    return;
}
void Comm::onReadyRead()
{
    while (this->udp->hasPendingDatagrams())
    {
        qint64 size = this->udp->pendingDatagramSize();
        QByteArray array;
        array.resize(static_cast<int>(size));
        QHostAddress ipAddr;
        quint16 port;
        qint64 len = this->udp->readDatagram(array.data(), size, &ipAddr, &port);
        if (len > 0)
        {
            static std::map<QString, std::pair<QByteArray, QString>> imgMap;
            HeadInfo *headInfo = reinterpret_cast<HeadInfo*>(array.data());
            if (headInfo->start == 0xaa)
            {
                switch (headInfo->type)
                {
                case ChatType::Login:
                    this->sendInfo(ipAddr);
                    break;
                case ChatType::Name:
                {
                    NameInfo *info = reinterpret_cast<NameInfo*>(array.data());
                    UserInfo userInfo{headInfo->id, ipAddr.toString(), info->name, ""};
                    emit this->nameInfo(userInfo);
                }
                    break;
                case ChatType::Image:
                {
                    ImageInfo *info = reinterpret_cast<ImageInfo*>(array.data());
                    imgMap[headInfo->id] = std::make_pair(info->imgMD5, info->img);
                }
                    break;
                case ChatType::Message:
                {
                    MessageInfo *info = reinterpret_cast<MessageInfo*>(array.data());
                    emit this->msgInfo(headInfo->id, info->msg);
                }
                    break;
                case ChatType::Logout:
                    emit this->logOut(headInfo->id);
                    break;
                default:
                    break;
                }
            }
            else
            {
                QByteArray md5 = QCryptographicHash::hash(array, QCryptographicHash::Md5);
                std::map<QString, std::pair<QByteArray, QString>>::const_iterator cit = imgMap.cbegin();
                while (cit != imgMap.cend())
                {
                    if (cit->second.first.compare(md5.toHex()) == 0)
                    {
                        QByteArray imageData = QByteArray::fromBase64(array);
                        QImage image;
                        image.loadFromData(imageData);
                        QString imagePath = QCoreApplication::applicationDirPath().append(QString("/%1.%2").arg(cit->first, cit->second.second));
                        if (QFile::exists(imagePath))
                            QFile::remove(imagePath);
                        image.save(imagePath);
                        imageData.clear();
                        emit this->imageInfo(cit->first, imagePath);
                    }
                    ++cit;
                }
            }
        }
        array.clear();
        array.squeeze();
    }
    return;
}
Comm::~Comm()
{
    HeadInfo info;
    strcpy(info.id, Single::id.toUtf8().data());
    info.type = ChatType::Logout;
    this->udp->writeDatagram(reinterpret_cast<char*>(&info), sizeof(HeadInfo), QHostAddress(this->multicast), this->port);
}
