#include "basiczombie.h"



basiczombie::basiczombie() : zombie()  // 显式调用父类构造函数
{
    //state 0行走，1攻击，2死亡，初始化基类为0
    hp = 400; // 设置基础僵尸的生命值为270
    atk = 100 * 33 / 1000; // 设置基础僵尸的攻击力为100（每秒攻击100点）
    speed = 20.0 * 33 / 1000; // 设置基础僵尸的速度为20.0像素/秒（33毫秒为帧间隔）
    originalSpeed =speed;
    setMovie(":/new/prefix1/ZombieWalk1.gif"); // 设置基础僵尸的行走动画
}

void basiczombie::advance(int phase)
{
    // 1. 先调用基类的advance，处理减速倒计时和速度恢复
    zombie::advance(phase);

    if (!phase)
        return;
    update(); // 更新基础僵尸的绘制
    if (hp <= 0) // 如果基础僵尸的生命值小于等于0，表示已经被击败
    {
        if (state < 2) // 如果基础僵尸的状态小于2，表示处于死亡状态
        {
            state = 2; // 将状态设置为2（死亡）
            setMovie(":/new/prefix1/ZombieDie.gif"); // 设置基础僵尸的死亡动画
            setHead(":/new/prefix1/ZombieHead.gif"); // 设置基础僵尸的头部掉落动画
        }


        else if (mQMovie->currentFrameNumber() == mQMovie->frameCount() - 1)
            delete this; // 如果基础僵尸的死亡动画播放完毕，删除基础僵尸对象
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
                    setMovie(":/new/prefix1/ZombieAttack.gif");
                }
                return; // 找到碰撞就返回，不执行移动
            }
        }
    }

    if (state) // 如果基础僵尸的状态不为0（行走状态）
    {
        state = 0; // 将状态设置为0（行走）
        setMovie(":/new/prefix1/ZombieWalk1.gif"); // 设置基础僵尸的行走动画
    }
    setX(x() - speed); // 更新基础僵尸的位置，让其向左移动
}
