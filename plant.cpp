#include "plant.h"
#include "zombie.h"

// 初始化静态全局变量
QSound* plant::plantSound = nullptr;
plant::plant() : QObject(), QGraphicsItem()  // 初始化所有基类
{
    hp = state = atk = counter = time = 0;
    mQMovie = nullptr;
    // 初始化音效（如果尚未初始化）
    if (plantSound == nullptr) {
        plantSound = new QSound(":/new/prefix2/plantplace.wav");
    }
}

plant::~plant()
{
    delete mQMovie;
}
void plant::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawImage(boundingRect(),mQMovie->currentImage()); //边界和当前帧数图像

}
void plant::setMovie(QString path)   //播放动画
{
    if(mQMovie)
        delete mQMovie;
    mQMovie=new QMovie(path);
    mQMovie->start();
}
QRectF plant::boundingRect()const  //返回矩形区域
{
    return QRect(-35,-35,70,70);
}
bool plant::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode)const
{
    Q_UNUSED(mode);
    return other->type() == zombie::Type && qFuzzyCompare(other->y(), y()) && qAbs(other->x() - x()) < 30;
    //判断是否是僵尸，判断是否是同一行，判断是否靠近植物
}
int plant::type()const
{
    return Type;
}


