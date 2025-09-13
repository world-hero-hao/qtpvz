#ifndef SHOVEL_H
#define SHOVEL_H

#include "other.h"

class shovel : public other
{
public:
    shovel();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    // 移除指定位置的植物
    // 注意：此方法在僵尸吃植物时直接调用可能会导致卡顿
    // 因为删除操作可能在游戏逻辑更新的关键帧中执行
    void removePlant(QPointF pos);

    // 新增方法：延迟移除植物，避免在关键帧中直接删除
    // 此方法将植物删除操作推迟到游戏逻辑更新完成后
    // 可显著减少铲除植物时的卡顿现象
    void schedulePlantRemoval(QGraphicsItem* plantItem);

private:
    // 新增：标记需要延迟移除的植物列表
    // 用于跟踪已计划移除但尚未执行的植物
    QList<QGraphicsItem*> plantsToRemove;
};

#endif // SHOVEL_H
