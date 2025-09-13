#include "mower.h"

Mower::Mower():other()
{
    flag = false;
    speed = 270.0 * 33 / 1000;
}

QRectF Mower::boundingRect() const
{
    return QRectF(-30, -30, 60, 60);
}

void Mower::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawPixmap(QRect(-30, -30, 60, 60), QPixmap(":/new/prefix1/LawnMower.png"));
}

bool Mower::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
// Qt 预留的虚函数，专门用来让开发者自定义碰撞规则，collidingItems()调用时使用自定义筛选结果
{
    Q_UNUSED(mode)
    return other->type() == zombie::Type && qFuzzyCompare(other->y(), y()) && qAbs(other->x() - x()) < 15;
}

void Mower::advance(int phase)
{
    if (!phase)
        return;
    update();
    QList<QGraphicsItem *> items = collidingItems();
    if (!items.empty())
    {
        flag = true;
        foreach (QGraphicsItem *item, items)
        {
            zombie *zom = qgraphicsitem_cast<zombie *>(item);
            zom->hp = 0;
        }
    }
    if (flag)
        setX(x() + speed);
    if (x() > 1069)
        delete this;
}
