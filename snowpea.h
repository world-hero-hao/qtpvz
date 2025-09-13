#ifndef SNOWPEA_H
#define SNOWPEA_H

#include "plant.h"

class snowpea : public plant
{
public:
    snowpea();
    void advance(int phase) override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
private slots:
    void shootSuperIcePea();  // 发射超级寒冰子弹的槽函数

private:
    int counter = 0;          // 普通寒冰子弹计数器（初始化避免随机值）
    QTimer *superIceTimer;    // 超级寒冰子弹计时器（15秒间隔）
};

#endif // SNOWPEA_H
