#ifndef PLANT_H
#define PLANT_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QMovie>
#include <QSound>
#include <QObject>

class plant : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 1};
    int hp;
    int state;
    int atk;
    int counter;
    int time;
    QMovie* mQMovie;
    // 添加静态全局变量用于种植音效
    static QSound* plantSound;

    int type()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)override;

    void setMovie(QString path);
    QRectF boundingRect()const override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape)const override;

    plant();
    virtual ~plant();
};

#endif // PLANT_H
