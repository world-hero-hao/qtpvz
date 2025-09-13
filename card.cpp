#include "card.h"
#include "shop.h"

const QMap<QString, int> card::map = {{"SunFlower", 0}, {"Peashooter", 1}, {"CherryBomb", 2}, {"WallNut", 3},
                                      {"SnowPea", 4}, {"PotatoMine", 5}, {"Repeater", 6}};
const QVector<QString> card::name = {"SunFlower", "Peashooter", "CherryBomb", "WallNut",
                                     "SnowPea", "PotatoMine", "Repeater"};
const QVector<int> card::cost = {50, 100, 150, 50, 175, 25, 200};
const QVector<int> card::cool = {227, 227, 606, 606, 227, 606, 227};

card::card(QString s):other()
{
    text = s;
    counter = 0;
}

QRectF card::boundingRect() const
{
    return QRectF(-50, -30, 100, 60);
}

void card::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{   //此函数QT自动每帧触发

    Q_UNUSED(option)
    Q_UNUSED(widget)
    //卡套
    painter->scale(0.6, 0.58);
    painter->drawPixmap(QRect(-50, -70, 100, 140), QPixmap(":/new/prefix1/Card.png"));
    painter->drawPixmap(QRect(-35, -42, 70, 70), QPixmap(":/new/prefix1/" + text + ".png"));

    //所需阳光显示
    QFont font;  //字体对象
    font.setPointSizeF(15);
    painter->setFont(font);
    painter->drawText(-30, 60, QString().sprintf("%3d", cost[map[text]]));

    if (counter < cool[map[text]])
    {   //冷却中
        QBrush brush(QColor(0, 0, 0, 200)); //纯黑
        painter->setBrush(brush);
        painter->drawRect(QRectF(-48, -68, 98, 132 * (1 - qreal(counter) / cool[map[text]])));
        //x,y,宽，高(随着帧率变化）
    }
}

void card::advance(int phase)
{

     //qt自带，帧率调用
    if (!phase)
        return;
    update();
    if (counter < cool[map[text]])
        ++counter;
}

void card::mousePressEvent(QGraphicsSceneMouseEvent *event)
{    //按
    Q_UNUSED(event)
    if (counter < cool[map[text]])   //冷却中
        event->setAccepted(false);   //拒绝
    shop *sh = qgraphicsitem_cast<shop *>(parentItem());   //获取当前植物栏信息
    if (cost[map[text]] > sh->sunnum)
        event->setAccepted(false);
    setCursor(Qt::ArrowCursor);//默认的鼠标指针形状
}

void card::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{    //拖动
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length()
        < QApplication::startDragDistance())//检查鼠标点击事件的起点和终点之间的距离是否小于系统设置的开始拖拽的距离阈值。
        //这是为了防止在点击时意外触发拖拽操作。
        return;
    QDrag *drag = new QDrag(event->widget());//创建一个新的拖拽对象QDrag，该对象用于实现拖拽操作过程。
    //（拖拽打包好的数据）
    QMimeData *mime = new QMimeData;//用于存储要拖拽的数据。（打包数据）
    QImage image(":/new/prefix1/" + text + ".png");  //告诉要拖拽的植物图标是谁”
    mime->setText(text);  //储存植物名称
    mime->setImageData(image);   //储存图像
    drag->setMimeData(mime);  //把对象信息传给拖拽
    drag->setPixmap(QPixmap::fromImage(image));//设置拖拽时跟随鼠标的图标（即拖拽过程中，鼠标旁边显示的植物小图）。
    drag->setHotSpot(QPoint(35, 35));//设置置拖拽图标的 “热点”（即图标与鼠标位置的对齐点）拖拽显示。

    drag->exec();//执行拖拽操作，开始拖拽。  场景会接收到拖拽释放事件，并执行获取信息和是否能种植
    setCursor(Qt::ArrowCursor);
}

void card::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{    //释放
    Q_UNUSED(event)
    setCursor(Qt::ArrowCursor);
}
