#include "cherry.h"
#include "zombie.h"
#include <QUrl>
#include <QDebug>

// 1. 初始化静态变量（全局唯一，程序启动即存在）
QSoundEffect* cherry::s_explosionSound = nullptr;
bool cherry::s_soundInitialized = false;
int cherry::s_explosionCount = 0;

// 2. 静态函数：初始化全局爆炸音效（只执行一次）
void cherry::initSounds()
{
    if (!s_soundInitialized) {
        s_explosionSound = new QSoundEffect();
        s_explosionSound->setSource(QUrl::fromLocalFile(":/new/prefix2/Boom.wav"));  // 加载樱桃爆炸音效
        s_explosionSound->setVolume(1.0f);  // 音量拉满（和原代码一致）
        s_soundInitialized = true;
    }
}

cherry::cherry() : plant()
{
    atk = 1800;
    hp = 800;
    state = 0;
    soundPlayed = false;  // 初始：当前樱桃未播放音效
    setMovie(":/new/prefix1/CherryBomb.gif");

    // 3. 构造函数中初始化全局音效（防止空指针，全局只执行一次）
    if (!s_soundInitialized) {
        initSounds();
    }
}

cherry::~cherry()
{
    // 无需删除静态音效实例（全局唯一，程序结束才销毁）
    // 仅在当前樱桃爆炸后，引用计数减1（避免计数残留）
    if (soundPlayed && s_explosionCount > 0) {
        s_explosionCount--;
    }
}

QRectF cherry::boundingRect() const
{
    // 爆炸时扩大碰撞范围（原逻辑不变）
    return state ? QRectF(-150, -150, 300, 300) : plant::boundingRect();
}

void cherry::advance(int phase)
{
    if (!phase) return;
    update();

    // 血量为0直接删除
    if (hp <= 0) {
        delete this;
        return;
    }

    // 4. 樱桃爆炸逻辑（核心：播放全局音效）
    // 状态0：爆炸动画未结束 → 进入爆炸状态
    if (state == 0 && mQMovie && mQMovie->currentFrameNumber() == mQMovie->frameCount() - 1) {
        state = 1;
        setMovie(":/new/prefix1/Boom.gif");

        // 5. 引用计数控制全局音效（和土豆/阳光完全一致）
        if (s_explosionSound && !soundPlayed) {
            s_explosionCount++;  // 计数+1：标记有樱桃在爆炸
            // 只有第一个爆炸的樱桃触发播放（QSoundEffect自动覆盖现有播放，无需Timer）
            if (s_explosionCount == 1) {
                s_explosionSound->play();
            }
            soundPlayed = true;  // 标记当前樱桃已播放音效（防止重复触发）
        }

        // 6. 爆炸伤害逻辑（原逻辑不变）
        QList<QGraphicsItem *> items = collidingItems();
        foreach (QGraphicsItem *item, items) {
            zombie *zom = qgraphicsitem_cast<zombie*>(item);
            if (zom) {
                zom->hp -= atk;
                if (zom->hp <= 0) {
                    // 死亡时强制清除所有冰冻状态
                   zom->isFrosted = false;
                   zom-> isSuperFrosted = false;
                    zom->state = 3;
                    zom->setMovie(":/new/prefix1/Burn.gif");
                }
            }
        }
    }
    // 状态1：爆炸动画播放完毕 → 删除樱桃
    else if (state == 1 && mQMovie && mQMovie->currentFrameNumber() == mQMovie->frameCount() - 1) {
        delete this;
    }
}

bool cherry::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode);
    // 爆炸范围：与僵尸距离小于160像素（原逻辑不变）
    return other->type() == zombie::Type && QLineF(pos(), other->pos()).length() < 160;
}
