#include "shop.h"

shop::shop():other()
{
    sunnum = 200;
    counter = 0;
    time = int(7.0 * 1000 / 33);
    card *car = nullptr;
    for (int i = 0; i < card::name.size(); ++i)
    {
        car = new card(card::name[i]);
        car->setParentItem(this);
        car->setPos(-157 + 65 * i, -2);
    }
}

QRectF shop::boundingRect() const
{
    return QRectF(-270, -45, 540, 90);
}

void shop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    //绘制卡片框
    painter->drawPixmap(QRect(-270, -45, 540, 90), QPixmap(":/new/prefix1/Shop.png"));

    //绘制阳光文本
    QFont font;
    font.setPointSizeF(15);
    painter->setFont(font);
    painter->drawText(QRectF(-255, 18, 65, 22), Qt::AlignCenter, QString::number(sunnum));
}

void shop::advance(int phase)
{
    if (!phase)
        return;
    update();
    if (++counter >= time)
    {
        counter = 0;
        scene()->addItem(new sun);
    }
}

//种下植物
void shop::addPlant(QString s, QPointF pos)
{
    QList<QGraphicsItem *> items = scene()->items(pos);  //获取点击位置的图形项
    foreach (QGraphicsItem *item, items)  //遍历所有图形项
        if (item->type() == plant::Type)
            return;
    sunnum -= card::cost[card::map[s]];//总阳光减去对应植物卡片的阳光
    plant *pl = nullptr;


    //根据卡片索引创建对应植物对象
    switch (card::map[s])
    {
    case 0:
        pl = new sunflower; break;
    case 1:
        pl = new pea; break;
    case 2:
        pl = new cherry; break;
    case 3:
        pl = new wallnut; break;
    case 4:
        pl = new snowpea; break;
    case 5:
        pl = new potato; break;
    case 6:
        pl = new repeater; break;
    }


    //放置植物和启动冷却
    pl->setPos(pos);
    scene()->addItem(pl);

    // 播放种植音效
    if (plant::plantSound) {
        plant::plantSound->play();
    }


    QList<QGraphicsItem *> child = childItems();
    foreach (QGraphicsItem *item, child)
    {
        card *car = qgraphicsitem_cast<card *>(item);
        if (car->text == s) //卡片名字对应传参名字
            car->counter = 0;
    }
}

