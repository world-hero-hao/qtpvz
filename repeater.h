#ifndef REPEATER_H
#define REPEATER_H

#include "plant.h"
#include "zombie.h"
#include "peashot.h"
#include <QTimer>  // 新增：引入计时器头文件

class repeater : public plant
{
public:
    repeater();
    void advance(int phase) override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
private slots:
    void shootSuperPeas();  // 新增：发射超级子弹的槽函数
private:
    int counter = 0;        // 新增：普通子弹计数器
    QTimer *superPeaTimer;  // 新增：超级子弹计时器
};

#endif // REPEATER_H
