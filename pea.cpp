#include "pea.h"
#include "zombie.h"
#include "peashot.h"
#include <QTimer>

pea::pea() : plant()
{
    hp = 200;         // 普通豌豆射手生命值
    atk = 30;         // 普通豌豆攻击力
    time = 42;        // 发射间隔（约1.4秒，33ms/帧 * 42 ≈ 1.4秒）
    counter = 0;      // 发射计数器初始化

    // 设置普通豌豆射手的动画
    setMovie(":/new/prefix1/Peashooter.gif");

    // 初始化超大豌豆计时器（每10秒发射一次）
    superPeaTimer = new QTimer(this);
    superPeaTimer->start(10000);  // 10秒间隔

    // 连接计时器信号到发射超大豌豆的槽函数
    connect(superPeaTimer, &QTimer::timeout, this, &pea::shootSuperPea);
}

void pea::advance(int phase)
{
    if (!phase) return;
    update();

    // 植物死亡时删除自身
    if (hp <= 0) {
        delete this;
        return;
    }

    // 普通豌豆发射逻辑（达到发射间隔且范围内有僵尸）
    if (++counter >= time) {
        counter = 0;
        if (!collidingItems().isEmpty()) {
            // 创建普通豌豆（非寒冰，非超大）
            peashot* newshot = new peashot(atk, false, false);
            newshot->setX(x() + 30);  // 从射手右侧发射
            newshot->setY(y());       // 与射手同一行
            scene()->addItem(newshot);
        }
    }
}

// 发射超大豌豆的槽函数
void pea::shootSuperPea()
{
    // 仅当植物存活且范围内有僵尸时发射
    if (hp > 0 && !collidingItems().isEmpty()) {
        // 创建超大豌豆（非寒冰，是超大）
        peashot* superShot = new peashot(atk, false, true);
        superShot->setX(x() + 30);  // 从射手右侧发射
        superShot->setY(y());       // 与射手同一行
        scene()->addItem(superShot);
    }
}

// 检测范围内是否有僵尸（用于判断是否需要发射豌豆）
bool pea::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode);
    // 仅检测同一行的僵尸
    return other->type() == zombie::Type && qFuzzyCompare(other->y(), y());
}
