#ifndef POTATO_H
#define POTATO_H

#include "plant.h"
#include <QSoundEffect>

class potato : public plant
{
public:
    potato();
    QRectF boundingRect() const override;
    void advance(int phase) override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
    ~potato() override;

    // 全局静态音效变量（和僵尸逻辑一致，无计时器）
    static QSoundEffect* s_explosionSound;   // 全局唯一爆炸音效实例
    static bool s_soundInitialized;          // 音效初始化标记
    static int s_explosionCount;             // 爆炸引用计数

    // 静态音效初始化函数
    static void initSounds();

private:
    bool soundPlayed;  // 标记当前土豆是否已播放过音效
};

#endif // POTATO_H
