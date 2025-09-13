#include "peashot.h"
#include "zombie.h"
#include <QRandomGenerator>

// 初始化静态变量
QSoundEffect* peashot::s_hitSound = nullptr;
bool peashot::s_soundInitialized = false;
bool peashot::s_isSoundPlaying = false;

// 静态音效初始化
void peashot::initSounds()
{
    if (!s_soundInitialized) {
        s_hitSound = new QSoundEffect();
        s_hitSound->setSource(QUrl::fromLocalFile(":/new/prefix2/plantattack.wav"));
        s_hitSound->setVolume(1.0f);
        s_soundInitialized = true;

        // 绑定音效播放完成信号
        QObject::connect(s_hitSound, &QSoundEffect::playingChanged, [=]() {
            if (!s_hitSound->isPlaying()) {
                s_isSoundPlaying = false;
            }
        });
    }
}

// 构造函数：初始化所有属性
peashot::peashot(int attack, bool isIcePea, bool isSuper)
    : other(), m_isIcePea(isIcePea), m_isSuper(isSuper)
{
    if (!s_soundInitialized) {
        initSounds();
    }

    // 超大豌豆攻击力为5倍
    atk = isSuper ? attack * 5 : attack;

    // 超大豌豆速度减慢（原速度的1/2）
    speed = (isSuper ? 150.0 : 300.0) * 33 / 1000;
}

// 清理音效
void peashot::clearAllHitSounds()
{
    if (s_hitSound) {
        s_hitSound->stop();
        s_isSoundPlaying = false;
    }
}

// 检查音效是否正在播放
bool peashot::isSoundPlaying()
{
    return s_isSoundPlaying;
}

// 边界矩形
QRectF peashot::boundingRect() const
{
    if (m_isSuper) {
        return QRectF(-24, -56, 50, 50); // 超大豌豆碰撞范围
    }
    return QRectF(-12, -28, 25, 25);   // 普通豌豆碰撞范围
}

// 统一使用自定义碰撞检测逻辑
bool peashot::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode);

    // 只检测与僵尸的碰撞
    if (other->type() != zombie::Type) {
        return false;
    }

    // 必须在同一行（y坐标接近）
    if (!qFuzzyCompare(other->y(), y())) {
        return false;
    }

    // 根据豌豆类型调整x轴检测范围
    if (m_isSuper) {
        // 超大豌豆检测范围更大
        return qAbs(other->x() - x()) < 50;
    } else {
        // 普通豌豆检测范围
        return qAbs(other->x() - x()) < 25;
    }
}

// 绘制
void peashot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 根据是否为寒冰豌豆选择不同图片
    QPixmap originalPix = QPixmap(m_isIcePea ? ":/new/prefix1/PeaSnow.png" : ":/new/prefix1/Pea.png");

    if (m_isSuper) {
        // 超大豌豆放大绘制
        painter->drawPixmap(QRect(-24, -56, 50, 50), originalPix.scaled(50, 50));
    } else {
        // 普通大小绘制
        painter->drawPixmap(QRect(-12, -28, 25, 25), originalPix);
    }
}

void peashot::advance(int phase)
{
    if (!phase)
        return;
    update();

    // 获取所有可能碰撞的对象
    QList<QGraphicsItem *> items = collidingItems();
    if (!items.isEmpty())
    {
        foreach (QGraphicsItem *item, items) {
            if (collidesWithItem(item, Qt::IntersectsItemShape)) {
                zombie *zom = qgraphicsitem_cast<zombie *>(item);
                if (zom) {
                    zom->hp -= atk;

                        // 寒冰效果触发（核心修复）
                        if (m_isIcePea) {
                            // 普通寒冰子弹：传递false → 触发减速
                            // 超级寒冰子弹：传递true → 触发禁移
                            zom->setFrosted(m_isSuper);
                        }


                    // 播放攻击音效
                    if (s_hitSound && !s_isSoundPlaying) {
                        s_isSoundPlaying = true;
                        s_hitSound->play();
                    }

                    delete this; // 命中后删除子弹
                    return;
                }
            }
        }
    }

    // 移动与边界删除
    setX(x() + speed);
    if (x() > 1069)
        delete this;
}
