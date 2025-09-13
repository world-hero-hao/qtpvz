#ifndef SUN_H
#define SUN_H

#include "other.h"
#include <QSoundEffect>  // 仅保留音效头文件，移除QTimer

class sun : public other
{
public:
    sun();                  // 自动阳光（从上方掉落）
    sun(QPointF pos);       // 向日葵产生的阳光
    ~sun() override;        // 析构函数释放资源

    // 核心重写函数
    QRectF boundingRect() const override;   // 碰撞检测边界
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;  // 绘制阳光
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;  // 点击收集
    void advance(int phase) override;  // 每帧更新（下落/自动消失）

    // 公有静态变量：移除计时器，只保留核心音效控制
    static QSoundEffect* s_collectSound;   // 全局唯一收集音效实例
    static bool s_soundInitialized;        // 音效是否已初始化的标记
    static int s_collectCount;             // 收集引用计数（防重复播放）

    // 静态音效初始化函数（全局统一创建实例）
    static void initSounds();

private:
    QMovie *movie;   // 阳光GIF动画
    QPointF dest;    // 阳光最终落点
    int counter;     // 计时器（记录存在时间，和音效无关）
    int time;        // 最大存在时间（超时自动消失）
    qreal speed;     // 下落速度
};

#endif // SUN_H
