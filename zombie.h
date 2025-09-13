#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QMovie>
#include <QSoundEffect>  // 改用QSoundEffect
#include <QObject>
#include <QTimer>

class zombie : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 2 };
    int hp;   //生命
    int state; //记录状态
    int atk;  //攻击
    qreal speed;  //移动
    QMovie* mQMovie;  //移动动画
    QMovie* mhead;  //死亡时候头部动画

    // 减速管理相关变量
    double originalSpeed;  // 保存僵尸的"初始速度"
    bool isFrosted;        // 标记是否处于减速状态
    int frostTimer;        // 减速计时器
    bool isSuperFrosted;   // 新增：标记是否被超级寒冰击中
    bool isEating;
    // 音效相关变量（改为静态）
    static QSoundEffect* s_eatSound;    // 静态吃植物音效
    static QTimer* s_eatSoundTimer;     // 静态音效定时器
    static bool s_soundInitialized;     // 音效初始化标志
    static int s_eatingCount;           // 新增：静态引用计数，跟踪正在进食的僵尸数量
    QList<QGraphicsItem*> lastCollidingItems;  // 缓存碰撞检测结果

    zombie();
    virtual ~zombie();

    // 新增：设置冰冻状态接口（区分普通/超级寒冰）
    void setFrosted(bool isSuperFrosted);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
    void setMovie(QString path);
    void setHead(QString path);
    int type() const override;
    void advance(int phase) override;
    // 静态初始化函数
    static void initSounds();
    // 在zombie.h中添加
};

#endif // ZOMBIE_H
