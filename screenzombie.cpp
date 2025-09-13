#include "screenzombie.h"

ScreenZombie::ScreenZombie()
{
    hp = 1350;
    atk = 100 * 33 / 1000;
    speed = 80.0 * 33 / 1000 / 4.7;
    originalSpeed =speed;
    //隐身状态
    //setMovie(":new/prefix1/ScreenZombieWalk.gif"); //正常
}

void ScreenZombie::advance(int phase)
{
    // 1. 先调用基类的advance，处理减速倒计时和速度恢复
    zombie::advance(phase);

    if (!phase)
        return;
    update();
    if (hp <= 0)
    {
        if (state < 2)
        {
            state = 2;
            setMovie(":/new/prefix1/ZombieDie.gif");
            setHead(":/new/prefix1/ZombieHead.gif");
        }
        else if (mQMovie->currentFrameNumber() == mQMovie->frameCount() - 1)
            delete this;
        return;
    }
    QList<QGraphicsItem *> items = collidingItems();
    if (!items.isEmpty()) // 先初步筛选
    {
        // 使用精确的碰撞检测函数进行二次筛选
        foreach (QGraphicsItem *item, items) {
            if (collidesWithItem(item, Qt::IntersectsItemShape)) {
                plant *pl = qgraphicsitem_cast<plant *>(item);
                pl->hp -= atk; // 减少植物的生命值

                if (state != 1) {
                    state = 1;
                    setMovie(":/new/prefix1/ScreenZombieAttack.gif");
                }
                return; // 找到碰撞就返回
            }
        }
    }
    if (state)
    {
        state = 0;
        setMovie(":/new/prefix1/ScreenZombieWalk.gif");
    }
    setX(x() - speed);
}
