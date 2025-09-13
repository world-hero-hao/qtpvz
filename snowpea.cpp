#include "snowpea.h"
#include "peashot.h"
#include "zombie.h"

// 构造函数：初始化寒冰豌豆射手属性和计时器
snowpea::snowpea() : plant()
{
    // 基础属性设置
    atk = 35;                  // 基础攻击力（超级子弹将为其5倍）
    hp = 300;                  // 寒冰豌豆射手生命值
    time = int(1.4 * 1000 / 33); // 普通子弹发射间隔（约1.4秒）
    counter = 0;               // 普通子弹计数器初始化

    // 设置寒冰豌豆射手动画
    setMovie(":/new/prefix1/SnowPea.gif");

    // 初始化超级寒冰子弹计时器（15秒间隔）
    superIceTimer = new QTimer(this);
    superIceTimer->start(15000); // 15秒 = 15000毫秒

    // 连接计时器信号到发射超级子弹的槽函数
    connect(superIceTimer, &QTimer::timeout, this, &snowpea::shootSuperIcePea);
}

// 帧更新函数：处理普通子弹发射和植物状态
void snowpea::advance(int phase)
{
    if (!phase) // 跳过准备阶段
        return;

    update(); // 触发重绘

    // 植物死亡处理
    if (hp <= 0)
    {
        delete this;
        return;
    }

    // 普通寒冰子弹发射逻辑
    if (++counter >= time)
    {
        counter = 0; // 重置计数器

        // 检测到僵尸时才发射
        if (!collidingItems().isEmpty())   //自动调用collidesWithItem
        {
            // 创建普通寒冰子弹：isIcePea=true，isSuper=false
            peashot *normalIcePea = new peashot(atk, true, false);
            normalIcePea->setX(x() + 30); // 右侧30像素处发射
            normalIcePea->setY(y());      // 保持同一行
            scene()->addItem(normalIcePea);
        }
    }
}

// 超级寒冰子弹发射槽函数（15秒触发一次）
void snowpea::shootSuperIcePea()
{
    // 仅当植物存活且有目标时发射
    if (hp > 0 && !collidingItems().isEmpty())
    {
        // 创建超级寒冰子弹：isIcePea=true，isSuper=true
        peashot *superIcePea = new peashot(atk, true, true);
        superIcePea->setX(x() + 30); // 与普通子弹同位置发射
        superIcePea->setY(y());      // 保持同一行
        scene()->addItem(superIcePea);
    }
}

// 碰撞检测：判断是否有同一行的僵尸（用于决定是否发射子弹）(自动调用）
bool snowpea::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode); // 忽略未使用参数

    // 仅检测同一行的僵尸（y坐标匹配）
    return other->type() == zombie::Type
           && qFuzzyCompare(other->y(), y()); // 严格单行判断
}
