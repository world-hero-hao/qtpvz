#include "repeater.h"

repeater::repeater() : plant()
{
    atk = 35;
    hp = 300;
    time = int(1.8 * 1000 / 33);  // 保持原发射间隔
    counter = 0;                  // 初始化计数器
    setMovie(":/new/prefix1/Repeater.gif");

    // 新增：初始化超级子弹计时器（15秒间隔）
    superPeaTimer = new QTimer(this);
    superPeaTimer->start(15000);  // 15秒 = 15000毫秒

    // 连接计时器到超级子弹发射函数
    connect(superPeaTimer, &QTimer::timeout, this, &repeater::shootSuperPeas);
}

void repeater::advance(int phase)
{
    if (!phase)
        return;
    update();

    if (hp <= 0) {
        delete this;
        return;
    }

    // 普通子弹发射逻辑（保持双发原始子弹）
    if (++counter >= time) {
        counter = 0;
        if (!collidingItems().isEmpty()) {
            // 第一发普通原始子弹
            peashot *pe1 = new peashot(atk, false, false);
            pe1->setX(x() + 32);
            pe1->setY(y());
            scene()->addItem(pe1);

            // 第二发普通原始子弹
            peashot *pe2 = new peashot(atk, false, false);
            pe2->setX(x() + 50);
            pe2->setY(y());
            scene()->addItem(pe2);
        }
    }
}

// 新增：超级子弹发射逻辑（15秒一次）
void repeater::shootSuperPeas()
{
    // 仅当存活且有目标时发射
    if (hp > 0 && !collidingItems().isEmpty()) {
        // 第一发：超级原始子弹（非寒冰，超级）
        peashot *superNormal = new peashot(atk, false, true);
        superNormal->setX(x() + 32);
        superNormal->setY(y());
        scene()->addItem(superNormal);

        // 第二发：超级寒冰子弹（寒冰，超级）
        peashot *superIce = new peashot(atk, true, true);
        superIce->setX(x() + 50);
        superIce->setY(y());
        scene()->addItem(superIce);
    }
}

bool repeater::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode)
    return other->type() == zombie::Type && qFuzzyCompare(other->y(), y());
}
