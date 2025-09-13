#ifndef PEASHOT_H
#define PEASHOT_H

#include "other.h"
#include <QSoundEffect>
#include <QTimer>

class peashot : public other
{
public:
    // 只保留一个构造函数，避免歧义
    peashot(int attack = 0, bool isIcePea = false, bool isSuper = false);
    QRectF boundingRect() const override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void advance(int phase) override;

    // 音效管理静态函数
    static void initSounds();
    static void clearAllHitSounds();
    static bool isSoundPlaying();

    static QSoundEffect* s_hitSound;
    static bool s_soundInitialized;
    static bool s_isSoundPlaying;

private:
    bool m_isIcePea;  // 是否为寒冰豌豆
    bool m_isSuper;   // 是否为超大豌豆
    int atk;          // 攻击力
    qreal speed;      // 移动速度
};

#endif // PEASHOT_H
