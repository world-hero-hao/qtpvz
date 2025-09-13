#include "sun.h"
#include "shop.h"
#include <QUrl>  // 处理音效文件路径

// 初始化静态变量（移除计时器，只初始化核心音效相关）
QSoundEffect* sun::s_collectSound = nullptr;
bool sun::s_soundInitialized = false;
int sun::s_collectCount = 0;

// 静态函数：初始化音效（全局只执行一次，无计时器）
void sun::initSounds()
{
    if (!s_soundInitialized) {
        // 创建全局唯一音效实例，加载阳光收集音效
        s_collectSound = new QSoundEffect();
        s_collectSound->setSource(QUrl::fromLocalFile(":/new/prefix2/suncollect.wav"));
        s_collectSound->setVolume(1.0f);  // 音效音量

        s_soundInitialized = true;  // 标记为已初始化
    }
}

// 构造函数1：自动阳光（从屏幕上方随机位置掉落）
sun::sun() : other()
{
    // 音效初始化（全局只执行一次，防止空指针）
    if (!s_soundInitialized) {
        initSounds();
    }

    // 阳光基础属性初始化（原有逻辑不变）
    dest = QPoint(200 + qrand() % 200, 200 + qrand() % 200);  // 随机落点
    setPos(QPointF(dest.x(), 70));  // 初始位置（上方y=70处）
    speed = 60.0 * 50 / 1000;       // 下落速度
    counter = 0;                    // 初始计时器为0
    time = int(10.0 * 1000 / 33);   // 最大存在时间（10秒）
    movie = new QMovie(":/new/prefix1/Sun.gif");  // 加载阳光动画
    movie->start();                 // 启动动画
    setAcceptedMouseButtons(Qt::LeftButton);  // 接受左键点击
}

// 构造函数2：向日葵产生的阳光（从植物位置附近生成）
sun::sun(QPointF pos)
{
    // 音效初始化（全局只执行一次）
    if (!s_soundInitialized) {
        initSounds();
    }

    // 阳光基础属性初始化（原有逻辑不变）
    dest = QPointF(pos.x() + qrand() % 30, pos.y() + qrand() % 30);  // 落点在植物周围
    setPos(QPointF(dest.x(), pos.y()));  // 初始位置（植物y坐标）
    speed = 60 * 50 / 1000;              // 下落速度
    counter = 0;                          // 初始计时器为0
    time = int(10.0 * 1000 / 33);         // 最大存在时间（10秒）
    movie = new QMovie(":/new/prefix1/Sun.gif");  // 加载阳光动画
    movie->start();                       // 启动动画
    setAcceptedMouseButtons(Qt::LeftButton);  // 接受左键点击
}

// 析构函数：释放动画资源+重置引用计数
sun::~sun()
{
    if (movie) {
        delete movie;
    }
    // 阳光被删除时，若已播放过音效，引用计数减1（避免计数残留）
    if (counter >= time && s_collectCount > 0) {
        s_collectCount--;
    }
}

// 定义阳光的碰撞检测范围（点击判定区域：70x70正方形）
QRectF sun::boundingRect() const
{
    return QRectF(-35, -35, 70, 70);
}

// 绘制阳光动画（显示GIF当前帧）
void sun::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawImage(boundingRect(), movie->currentImage());
}

// 鼠标点击：收集阳光+播放音效（核心修改：无计时器，仅用引用计数）
void sun::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    // 1. 增加玩家阳光数量（原有逻辑不变）
    shop *sh = qgraphicsitem_cast<shop *>(scene()->items(QPointF(300, 15))[0]);
    if (sh) {
        sh->sunnum += 25;
    }

    // 2. 引用计数控制音效（和僵尸/樱桃逻辑完全一致）
    if (s_collectSound) {
        s_collectCount++;  // 计数+1：标记有阳光在收集
        // 只有第一次收集（计数从0→1）时播放音效
        // QSoundEffect会自动覆盖现有播放，无需计时器控制间隔
        if (s_collectCount == 1) {
            s_collectSound->play();
        }
    }

    // 3. 标记阳光即将消失（下一帧删除）
    counter = time;
    event->setAccepted(true);
}

// 每帧更新：处理阳光下落和自动消失（原有逻辑不变）
void sun::advance(int phase)
{
    if (!phase) {
        return;
    }
    update();  // 触发重绘（更新动画帧）

    // 超时或已收集：删除阳光
    if (++counter >= time) {
        delete this;
    }
    // 未到落点：继续下落
    else if (y() < dest.y()) {
        setY(y() + speed);
    }
}
