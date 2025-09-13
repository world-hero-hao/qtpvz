#ifndef CARD_H
#define CARD_H

#include "other.h"

class card : public other
{
public:
    int counter;  //判断是否冷却
    QString text;  //储存植物名称
    card(QString s);
    const static QMap<QString, int> map; //索引
    const static QVector<QString> name; //名字
    const static QVector<int> cost;   //费用
    const static QVector<int> cool;  //冷却
    QRectF boundingRect() const override; //卡片的矩形范围
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void advance(int phase) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override; //按下
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;  //移动
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;  //松开
};

#endif // CARD_H
