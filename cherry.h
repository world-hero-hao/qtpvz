#ifndef CHERRY_H
#define CHERRY_H

#include "plant.h"
#include <QSoundEffect>

class cherry : public plant
{
    Q_OBJECT

public:
    cherry();
    QRectF boundingRect() const override;
    void advance(int phase) override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
    ~cherry() override;

    // 1. 全局静态音效变量（和阳光/土豆/僵尸逻辑一致）
    static QSoundEffect* s_explosionSound;   // 全局唯一爆炸音效实例
    static bool s_soundInitialized;          // 音效初始化标记
    static int s_explosionCount;             // 爆炸引用计数（防重复播放）

    // 2. 静态音效初始化函数（全局统一创建）
    static void initSounds();

private:
    bool soundPlayed;  // 标记当前樱桃是否已播放过音效（防止单个樱桃重复触发）
};

#endif // CHERRY_H
