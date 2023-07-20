#include <QPainterPath>
#include "circlehead.h"
QString CircleHead::getSource() const noexcept
{
    return this->source;
}
void CircleHead::setSource(const QString &source) noexcept
{
    this->source = source;
    emit this->sourceChanged();
    this->update();
    return;
}
void CircleHead::paint(QPainter *painter) noexcept
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(0, 0, this->width(), this->height());
    if (this->source.isEmpty()) {
        painter->fillPath(path, Qt::gray);
    } else {
        QPixmap pixmap(this->source);
        painter->setClipPath(path);
        painter->drawPixmap(QRect(0, 0, this->width(), this->height()), pixmap);
    }
    painter->restore();
    return;
}