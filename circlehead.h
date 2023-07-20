#ifndef CIRCLEHEAD_H
#define CIRCLEHEAD_H
#include <QPainter>
#include <QQuickPaintedItem>
/*!
@brief 圆形头像类
*/
class CircleHead : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ getSource WRITE setSource NOTIFY sourceChanged)
public:
    using QQuickPaintedItem::QQuickPaintedItem;
    QString getSource() const noexcept;
    void setSource(const QString&) noexcept;
    virtual void paint(QPainter*) noexcept override;
private:
    QString source;
signals:
    void sourceChanged();
};
#endif