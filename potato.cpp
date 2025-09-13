#include "potato.h"
#include "zombie.h"
#include <QUrl>

// 初始化静态变量（无计时器）
QSoundEffect* potato::s_explosionSound = nullptr;
bool potato::s_soundInitialized = false;
int potato::s_explosionCount = 0;

// 静态音效初始化
void potato::initSounds()
{
    if (!s_soundInitialized) {
        s_explosionSound = new QSoundEffect();
        s_explosionSound->setSource(QUrl::fromLocalFile(":/new/prefix2/Potato.wav"));
        s_explosionSound->setVolume(1.0f);
        s_soundInitialized = true;
    }
}

potato::potato() : plant()
{
    atk = 1800;
    hp = 300;
    state = 0;
    soundPlayed = false;
    time = int(10.0 * 1000 / 33);
    setMovie(":/new/prefix1/PotatoMine1.gif");

    // 初始化音效
    if (!s_soundInitialized) {
        initSounds();
    }
}

potato::~potato()
{
    // 不删除静态音效实例
}

QRectF potato::boundingRect() const
{
    return state == 2 ? QRectF(-75, -75, 150, 150) : plant::boundingRect();
}

void potato::advance(int phase)
{
    if (!phase) return;
    update();

    if (hp <= 0) {
        delete this;
    }
    else if (state == 0 && ++counter >= time) {
        state = 1;
        counter = 0;
        time = int(1.0 * 1000 / 33);
        setMovie(":/new/prefix1/PotatoMine.gif");
    }
    else if (state == 1 && ++counter >= time) {
        counter = 0;
        QList<QGraphicsItem *> items = collidingItems();
        if (!items.isEmpty()) {
            state = 2;
            setMovie(":/new/prefix1/PotatoMineBomb.gif");

            // 音效播放逻辑（仅用引用计数，无计时器，对齐僵尸）
            if (s_explosionSound && !soundPlayed) {
                s_explosionCount++;
                // 只有第一个爆炸的土豆触发播放（QSoundEffect会自动覆盖现有播放）
                if (s_explosionCount == 1) {
                    s_explosionSound->play();
                }
                soundPlayed = true;
            }

            // 伤害逻辑
            foreach (QGraphicsItem *item, items) {
                zombie *zom = qgraphicsitem_cast<zombie *>(item);
                if (zom) {
                    zom->hp -= atk;
                    if (zom->hp <= 0) {
                        delete zom;
                    }
                }
            }
        }
    }
    else if (state == 2 && mQMovie->currentFrameNumber() == mQMovie->frameCount() - 1) {
        // 爆炸结束后，引用计数减1
        if (soundPlayed && s_explosionCount > 0) {
            s_explosionCount--;
        }
        delete this;
    }
}

bool potato::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode);
    return other->type() == zombie::Type && qFuzzyCompare(other->y(), y()) && qAbs(other->x() - x()) < 50;
}
