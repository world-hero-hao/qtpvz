#ifndef PEA_H
#define PEA_H

#include "plant.h"

class pea : public plant
{
public:
    pea();
    void advance(int phase) override;
   bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
private slots:
    void shootSuperPea(); // 发射超大豌豆的槽函数
private:
    int counter = 0; // 普通豌豆计数器
    QTimer *superPeaTimer; // 超大豌豆计时器

};

#endif // PEA_H
