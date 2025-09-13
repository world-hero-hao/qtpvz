#include "shovel.h"
#include "plant.h"
#include <QGraphicsScene>
#include <QApplication>
#include <QDrag>
#include <QTimer>

// 构造函数
shovel::shovel():other()
{
    // 保持原有构造函数不变
    // 如果需要初始化plantsToRemove列表，可以在这里进行
}

// 返回铲子的边界矩形
QRectF shovel::boundingRect() const
{
    // 返回一个固定大小的矩形作为铲子的边界
    return QRectF(-40, -40, 80, 80);
}

// 绘制铲子
void shovel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 忽略未使用的参数
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 绘制铲子的背景和图标
    painter->drawPixmap(QRect(-40, -40, 80, 80), QPixmap(":/new/prefix1/ShovelBank.png"));
    painter->drawPixmap(QRect(-35,-35,70,70), QPixmap(":/new/prefix1/Shovel.png"));
}

// 鼠标按下事件处理
void shovel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // 忽略事件参数
    Q_UNUSED(event);

    // 设置光标为箭头形状
    setCursor(Qt::ArrowCursor);
}

// 鼠标移动事件处理 - 实现拖拽功能
void shovel::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 创建拖动操作对象
    QDrag* drag = new QDrag(event->widget());

    // 创建MIME数据用于存储拖动信息
    QMimeData* data = new QMimeData;
    data->setText("shovel"); // 设置标识文本

    // 加载并缩放铲子图片
    QPixmap originalPixmap(":/new/prefix1/Shovel.png");
    QPixmap scaledPixmap = originalPixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 设置拖动数据
    drag->setMimeData(data);
    drag->setPixmap(scaledPixmap); // 设置拖动时显示的图片
    drag->setHotSpot(QPoint(40, 40)); // 设置热点位置（光标位置）

    // 执行拖动操作
    drag->exec();

    // 恢复光标形状
    setCursor(Qt::ArrowCursor);
}

// 鼠标释放事件处理
void shovel::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // 忽略事件参数
    Q_UNUSED(event);

    // 恢复光标形状
    setCursor(Qt::ArrowCursor);
}

// 移除指定位置的植物
// 注意：此方法在僵尸吃植物时直接调用可能会导致卡顿
// 因为删除操作可能在游戏逻辑更新的关键帧中执行
void shovel::removePlant(QPointF pos)
{
    // 优化：使用itemAt方法替代items()遍历，减少性能开销
    // itemAt只返回指定位置最顶层的项目，而不是所有项目
    QGraphicsItem* item = scene()->itemAt(pos, QTransform());

    // 检查找到的项目是否为植物类型
    if (item && item->type() == plant::Type)
    {
        // 直接删除植物项目
        // 注意：在僵尸吃植物时直接删除可能会导致卡顿
        // 因为游戏可能正在处理僵尸与植物的交互
        delete item;
        return;
    }

    // 如果没有找到植物，尝试在小范围内搜索
    // 创建一个小的搜索区域(10x10像素)，避免全场景遍历
    // 这比使用无边界搜索更高效
    QRectF searchArea(pos.x() - 5, pos.y() - 5, 10, 10);
    QList<QGraphicsItem*> items = scene()->items(searchArea);

    // 遍历找到的项目，寻找植物
    for (QGraphicsItem* item : items)
    {
        if (item->type() == plant::Type)
        {
            // 找到植物，删除并返回
            delete item;
            return;
        }
    }

    // 如果到这里还没有返回，说明指定位置没有植物
}

// 新增方法：延迟移除植物，避免在关键帧中直接删除
// 此方法将植物删除操作推迟到游戏逻辑更新完成后
// 可显著减少铲除植物时的卡顿现象
void shovel::schedulePlantRemoval(QGraphicsItem* plantItem)
{
    // 检查植物项目是否有效
    if (!plantItem || !plantItem->scene()) {
        return; // 无效项目，直接返回
    }

    // 将植物添加到待移除列表
    plantsToRemove.append(plantItem);

    // 使用单次定时器延迟移除操作
    // 延迟时间为0毫秒，意味着将在当前事件循环结束后执行
    // 这样可以将删除操作推迟到游戏逻辑更新完成后
    QTimer::singleShot(0, this, [this, plantItem]() {
        // 检查植物是否仍然存在于待移除列表中且场景中
        // 这可以防止重复删除或删除已不存在的项目
        if (plantsToRemove.contains(plantItem) && plantItem->scene())
        {
            // 从待移除列表中移除植物
            plantsToRemove.removeOne(plantItem);

            // 从场景中移除植物项目
            plantItem->scene()->removeItem(plantItem);

            // 删除植物对象，释放内存
            delete plantItem;
        }
    });
}
