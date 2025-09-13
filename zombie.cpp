#include "zombie.h"
#include "plant.h"

// 初始化静态变量（新增 s_eatingCount 初始化为 0）
QSoundEffect* zombie::s_eatSound = nullptr;
QTimer* zombie::s_eatSoundTimer = nullptr;
bool zombie::s_soundInitialized = false;
int zombie::s_eatingCount = 0;  // 初始化引用计数为 0

// 静态音效初始化
void zombie::initSounds()
{
    if (!s_soundInitialized) {
        s_eatSound = new QSoundEffect();
        s_eatSound->setSource(QUrl::fromLocalFile(":/new/prefix2/Eat.wav"));
        s_eatSound->setVolume(0.7f);

        s_eatSoundTimer = new QTimer();
        s_eatSoundTimer->setInterval(1000);

        s_soundInitialized = true;
    }
}

zombie::zombie():QObject(), QGraphicsItem()
{
    hp = state = atk = 0;
    speed = 0.0;
    mQMovie = mhead = nullptr;
    // 初始化减速管理变量
    originalSpeed = 0.0;  // 子类需同步为具体初始速度
    isFrosted = false;    // 初始未减速
    frostTimer = 0;       // 减速计时器初始为 0（修复：原代码是 int frostTimer=0; 局部变量，改为成员变量赋值）
    isEating = false;     // 初始未进食
    isSuperFrosted = false; // 新增：初始未被超级寒冰击中
    // 初始化静态音效（如果未初始化）
    if (!s_soundInitialized) {
        initSounds();
    }
}
// 新增：设置冰冻状态（区分普通/超级寒冰）
void zombie::setFrosted(bool isSuper)
{
    // 关键逻辑：如果当前处于于超级冰冻状态，普通普通寒冰弹的请求（isSuperFrosted=false）
    if (this->isSuperFrosted && !isSuper) {
        return; // 直接响应普通寒冰子弹，保留超级冰冻状态
    }

    isFrosted = true;                // 开启冰冻变色效果
    frostTimer = 60;                 // 重置计时器为2秒（60帧）

    if (isSuper) {
        // 超级寒冰：禁止移动（速度=0）
        speed = 0.0;
        isSuperFrosted = true;       // 标记为超级冰冻
    } else {
        // 普通寒冰：减速（速度=原速度的2/3）
        speed = originalSpeed * 2.0 / 3.0;
        isSuperFrosted = false;      // 标记为普通冰冻
    }
}


zombie::~zombie()
{
    delete mQMovie;
    delete mhead;
    // 新增：析构时检查是否仍在进食，若在则减少计数并重置音效
    if (isEating && s_soundInitialized && s_eatingCount > 0) {
        s_eatingCount--;
        // 若最后一只进食僵尸被析构，停止音效并重置循环次数
        if (s_eatingCount == 0 && s_eatSound) {
            s_eatSound->stop();
            s_eatSound->setLoopCount(1); // 重置为单次播放，避免下次播放时默认循环
        }
        isEating = false; // 标记为未进食，避免重复处理
    }

}

QRectF zombie::zombie::boundingRect() const
{
    return QRectF(-80, -100, 200, 140); // 边界矩形
}

void zombie::advance(int phase)
{
    if (!phase) return;

    // -------------------------- 1. 死亡状态处理（核心修改）--------------------------
    if (state == 2) {  // state=2 为死亡状态
        // 死亡时强制清除所有冰冻状态
        isFrosted = false;
        isSuperFrosted = false;
        if (isEating) { // 如果死亡前正在进食，需退出进食队列
            isEating = false;
            if (s_eatingCount > 0) {
                s_eatingCount--; // 引用计数减 1
                // 只有最后一只进食的僵尸死亡，才停止全局音效
                if (s_eatingCount == 0 && s_eatSound) {
                    s_eatSound->stop();
                    s_eatSound->setLoopCount(1);
                }
            }
        }
        return; // 死亡后直接退出，不执行后续碰撞检测和音效逻辑
    }

    // 2. 减速冰冻计时器逻辑（核心修改）
    if (isFrosted) {
        if (frostTimer > 0) {
            frostTimer--; // 计时器递减
            // 冰冻状态下动画减速（视觉效果）
            if (mQMovie) mQMovie->setSpeed(50);
            if (mhead) mhead->setSpeed(50);
        }
        else {
            // 计时结束，恢复正常状态
            isFrosted = false;
            isSuperFrosted = false; // 重置超级冰冻标记
            speed = originalSpeed;  // 恢复原速度
            if (mQMovie) mQMovie->setSpeed(100);
            if (mhead) mhead->setSpeed(100);
        }
    }
    // -------------------------- 3. 碰撞检测（保持不变）--------------------------
    QList<QGraphicsItem*> items = collidingItems();
    bool collisionDetected = false;  // 临时检测当前是否碰撞

    foreach (QGraphicsItem *item, items) {
        if (collidesWithItem(item, Qt::IntersectsItemShape)) {
            collisionDetected = true;
            break; // 只要碰撞到一个植物，就判定为进食
        }
    }

    // -------------------------- 4. 音效逻辑（新增引用计数同步）--------------------------
    if (collisionDetected) {
        // 从“未进食”变为“进食”：启动音效（仅第一只僵尸启动）
        if (!isEating) {
            isEating = true;
            s_eatingCount++; // 引用计数加 1
            if (s_eatSound && s_eatingCount == 1) { // 只有第一只进食的僵尸，才播放音效
                s_eatSound->setLoopCount(QSoundEffect::Infinite);
                s_eatSound->play();
            }
        }
    } else {
        // 从“进食”变为“未进食”：停止音效（仅最后一只僵尸停止）
        if (isEating) {
            isEating = false;
            if (s_eatingCount > 0) {
                s_eatingCount--; // 引用计数减 1
                // 只有最后一只僵尸停止进食，才停止全局音效
                if (s_eatingCount == 0 && s_eatSound) {
                    s_eatSound->stop();
                    s_eatSound->setLoopCount(1);
                }
            }
        }
    }
}

// -------------------------- 其他函数（paint、collidesWithItem 等保持不变）--------------------------
void zombie::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!mQMovie) {
        return;
    }
    QImage image = mQMovie->currentImage();

    if (isFrosted&& state != 2)
    {
        if (state != 2)
            mQMovie->setSpeed(50);
        int w = image.width();
        int h = image.height();
        for (int i = 0; i < h; ++i)
        {
            uchar *line = image.scanLine(i);
            for (int j = 5; j < w - 5; ++j)
                line[j << 2] = 200;
        }
    }

    painter->drawImage(QRectF(-70, -100, 140, 140), image);

    if (mhead)
    {
        image = mhead->currentImage();
        if (isFrosted)
        {
            int w = image.width();
            int h = image.height();
            for (int i = 0; i < h; ++i)
            {
                uchar *line = image.scanLine(i);
                for (int j = 5; j < w - 5; ++j)
                    line[j << 2] = 200;
            }
        }
        painter->drawImage(QRectF(0, -100, 140, 140), image);
    }
}

bool zombie::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode)
    // 仅与植物碰撞，且y坐标接近、x坐标差小于30
    return other->type() == plant::Type && qFuzzyCompare(other->y(), y()) && qAbs(other->x() - x()) < 30;
}

void zombie::setMovie(QString path)
{
    if (mQMovie)
        delete mQMovie;
    mQMovie = new QMovie(path);
    mQMovie->start();
}

void zombie::setHead(QString path)
{
    if (mhead)
        delete mhead;
    mhead = new QMovie(path);
    mhead->start();
}

int zombie::type() const
{
    return Type;
}
