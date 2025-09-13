#include "game.h"
#include "shop.h"
#include "map.h"
#include "shovel.h"
#include "basiczombie.h"
#include "conezombie.h"
#include "bucketzombie.h"
#include "screenzombie.h"
#include "footballzombie.h"
#include "zombie.h"
#include "mower.h"
#include <QPixmap>
#include <QSoundEffect>  // 替换 QSound
#include <QSlider>       // 音量滑块
#include <QLabel>        // 音量标签

game::game(QWidget *parent)
    : QWidget{parent}, isPaused(false)  // 初始化列表，设置初始状态为未暂停
{
    // ==================== 声音系统初始化 ====================
    // 使用 QSoundEffect 替代 QSound，提供更好的音量控制
    mQSound = new QSoundEffect(this);
    mQSound->setSource(QUrl("qrc:/new/prefix2/white day.wav"));  // 使用 qrc 路径加载背景音乐
    mQSound->setLoopCount(QSoundEffect::Infinite);  // 无限循环播放
    mQSound->setVolume(0.5);  // 默认音量 50%

    mWinSound = new QSoundEffect(this);
    mWinSound->setSource(QUrl("qrc:/new/prefix2/victory.wav"));  // 胜利音乐
    mWinSound->setVolume(0.5);  // 默认音量 50%

    mQTimer = new QTimer(this); // 游戏主计时器，驱动所有游戏逻辑

    // ==================== 窗口和场景设置 ====================
    resize(900, 600);  // 设置窗口初始大小
    setMinimumSize(400, 300);  // 设置最小窗口大小

    scene = new QGraphicsScene(this); // 创建游戏场景，管理所有游戏元素
    scene->setSceneRect(150, 0, 900, 600);  // 设置场景矩形（X偏移150，用于显示背景）
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);  // 禁用索引，提高性能

    // 创建视图用于显示场景
    view = new QGraphicsView(scene, this);
    view->setFrameStyle(QFrame::NoFrame);  // 无边框
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏水平滚动条
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);    // 隐藏垂直滚动条

    // 设置游戏背景
    QPixmap bgPixmap(":/new/prefix1/Background.jpg");
    if (bgPixmap.isNull()) {
        view->setBackgroundBrush(Qt::lightGray);  // 如果背景加载失败，使用灰色
    } else {
        view->setBackgroundBrush(bgPixmap);  // 设置背景图片
    }

    // ==================== 游戏引导系统 ====================
    // 初始化文字提示图片（游戏开始前的引导）
    textPixmaps << QPixmap(":/new/prefix1/write1.png")  // 第一张：准备...
                << QPixmap(":/new/prefix1/write2.png")  // 第二张：安放...
                << QPixmap(":/new/prefix1/write3.png"); // 第三张：植物！

    textItem = new QGraphicsPixmapItem;
    // 计算居中位置
    int firstImgWidth = textPixmaps[0].width();
    int firstImgHeight = textPixmaps[0].height();
    int fixedCenterX = scene->width() / 2 - firstImgWidth / 2;
    int fixedCenterY = scene->height() / 2 - firstImgHeight / 2;
    textItem->setPos(fixedCenterX, fixedCenterY);
    scene->addItem(textItem);
    textItem->hide();  // 初始隐藏
    currentTextIndex = 0;

    textShowTimer = new QTimer(this);
    textShowTimer->setInterval(1000);  // 每张图片显示1秒
    connect(textShowTimer, &QTimer::timeout, this, &game::showNextTextImage);

    // ==================== 最后一波提示系统 ====================
    finalWaveTextPixmap = QPixmap(":/new/prefix1/write4.png");
    finalWaveTextItem = new QGraphicsPixmapItem();

    // 适当放大提示图片（保持比例）
    qreal scaleFactor = 1.5;  // 调整为1.5倍，原来1.8可能太大
    int scaledWidth = finalWaveTextPixmap.width() * scaleFactor;
    int scaledHeight = finalWaveTextPixmap.height() * scaleFactor;
    QPixmap scaledPixmap = finalWaveTextPixmap.scaled(
        scaledWidth, scaledHeight,
        Qt::KeepAspectRatio, Qt::SmoothTransformation);
    finalWaveTextItem->setPixmap(scaledPixmap);

    // 计算居中位置 - 使用场景的实际尺寸
    QRectF sceneRect = scene->sceneRect();  // 获取场景的实际矩形
    qreal centerX = sceneRect.x() + (sceneRect.width() - scaledPixmap.width()) / 2;
    qreal centerY = sceneRect.y() + (sceneRect.height() - scaledPixmap.height()) / 2;
    finalWaveTextItem->setPos(centerX, centerY);

    scene->addItem(finalWaveTextItem);
    finalWaveTextItem->hide();
    isFinalWaveTextShowing = false;

    finalWaveTextTimer = new QTimer(this);  //最后一波计时器
    finalWaveTextTimer->setSingleShot(true);  // 单次触发
    finalWaveTextTimer->setInterval(4000);    // 显示4秒
    connect(finalWaveTextTimer, &QTimer::timeout, this, &game::hideFinalWaveText);

    // ==================== 游戏开始纸条提示 ====================
    noteItem = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/ZombieNote1.png"));
    noteItem->setPos(400, 200);  // 设置显示位置
    scene->addItem(noteItem);

    noteShowTimer = new QTimer(this);
    noteShowTimer->setSingleShot(true);  // 单次触发
    noteShowTimer->setInterval(3000);    // 显示3秒
    connect(noteShowTimer, &QTimer::timeout, this, &game::hideNoteAndStartGame);

    mQTimer->stop();  // 先暂停游戏主计时器
    noteShowTimer->start();  // 开始显示纸条

    // ==================== 游戏状态管理 ====================
    currentWave = 1;      // 初始波次
    isFinalWave = false;  // 未触发最后一波

    winCloseTimer = new QTimer(this);
    winCloseTimer->setSingleShot(true);
    connect(winCloseTimer, &QTimer::timeout, this, &game::closeWindowAfterWin);

    finalWaveTimer = new QTimer(this);
    finalWaveTimer->setInterval(5000);  // 5秒间隔
    connect(finalWaveTimer, &QTimer::timeout, this, &game::generateSecondFinalWave);
    finalWaveBatch = 0;  // 最后一波批次计数

    // ==================== 游戏界面元素 ====================
    // 商店界面
    shop *sh = new shop;
    sh->setPos(520, 45);
    scene->addItem(sh);

    // 铲子工具
    shovel *sho = new shovel;
    sho->setPos(830, 40);
    scene->addItem(sho);

    // 地图背景
    Map *map = new Map;
    map->setPos(618, 326);
    scene->addItem(map);

    // 割草机防御系统（每行一个）
    for (int i = 0; i < 5; ++i) {
        Mower *mower = new Mower;
        mower->setPos(215, 120 + 95 * i);
        scene->addItem(mower);
    }

    // ==================== 游戏逻辑连接 ====================
    // 连接计时器到场景动画
    connect(mQTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    // 连接计时器到僵尸生成
    connect(mQTimer, &QTimer::timeout, this, &game::addZombie);
    // 连接计时器到游戏状态检查
    connect(mQTimer, &QTimer::timeout, this, &game::check);

    // ==================== 用户界面控件 ====================
    // 先创建暂停按钮（必须在滑块和标签之前）
    pauseResumeButton = new QPushButton("暂停", this);
    pauseResumeButton->setGeometry(width() - 110, 10, 100, 30);  // 右上角位置
    pauseResumeButton->setStyleSheet(R"(
        QPushButton {
            background-image: url(:/new/prefix1/Button.png);
            background-repeat: no-repeat;
            background-position: center;
            background-color: transparent;
            border: none;
            color: white;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: rgba(255,255,255,50);  // 鼠标悬停效果
        }
    )");
    connect(pauseResumeButton, &QPushButton::clicked, this, &game::togglePauseResume);

    // 音量控制滑块 - 优化样式和层级
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setGeometry(width() - 110, 100, 100, 30);  // 对应标签位置向下调整
    volumeSlider->raise();  // 确保滑块在最上层，不会被其他元素遮挡
    volumeSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            border: 2px solid #FFFFFF;
            height: 8px;
            background: rgba(102, 102, 102, 0.8);  // 半透明灰色，减少遮挡
            margin: 2px 0;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #FF0000;
            border: 2px solid #FFFFFF;
            width: 18px;
            margin: -5px 0;
            border-radius: 9px;
            box-shadow: 0 0 3px rgba(0,0,0,0.5);  // 增加阴影，更突出
        }
        QSlider::handle:horizontal:hover {
            background: #FF5555;  // 鼠标悬停效果，提升交互感
        }
    )");


    connect(volumeSlider, &QSlider::valueChanged, this, &game::updateVolume);  // 重要：连接信号！

    // 音量标签
    volumeLabel = new QLabel("音量", this);
    volumeLabel->setGeometry(width() - 110, 60, 100, 30);
    volumeLabel->setStyleSheet("color: yellow; font-size: 14px; font-weight: bold; background: rgba(0,0,0,100);");
    volumeLabel->setAlignment(Qt::AlignCenter);  // 文字居中

    // 确保所有控件可见
    pauseResumeButton->show();
    volumeSlider->show();
    volumeLabel->show();

    // ==================== 波次进度条 ====================
    waveProgressBar = new QProgressBar(this);
    waveProgressBar->setRange(0, FINAL_WAVE);  // 范围改为0到最终波次
    waveProgressBar->setValue(0);              // 初始值为0，显示"波次: 0/20"
    waveProgressBar->setGeometry(width() - 120, 150, 100, 35);  // 加宽加高：110x35
    waveProgressBar->setStyleSheet(R"(
    QProgressBar {
        border: 3px solid #FFD700;           /* 金色边框 */
        border-radius: 8px;                  /* 更大的圆角 */
        text-align: center;
        background: qlineargradient(         /* 背景渐变 */
            x1: 0, y1: 0,
            x2: 0, y2: 1,
            stop: 0 #2A2A2A,
            stop: 1 #000000
        );
        font-weight: bold;
        font-size: 14px;                     /* 更大的字体 */
        color: #FFFFFF;                      /* 白色文字 */
        padding: 3px;                        /* 增加内边距 */
        height: 30px;                        /* 明确高度 */
    }
    QProgressBar::chunk {
        background: qlineargradient(         /* 鲜艳的红-橙-黄渐变 */
            x1: 0, y1: 0.5,
            x2: 1, y2: 0.5,
            stop: 0 #FF0000,                 /* 红色 */
            stop: 0.3 #FF5500,               /* 橙色 */
            stop: 0.6 #FFAA00,               /* 黄橙色 */
            stop: 1 #FFFF00                  /* 黄色 */
        );
        border-radius: 6px;                  /* 与边框匹配的圆角 */
        border: 2px solid #FF9900;           /* 橙色边框 */
        margin: 1px;                         /* 外边距 */
    }
)");
    waveProgressBar->setFormat("波次: %v/%m");    // 显示格式：当前值/最大值
    waveProgressBar->setAlignment(Qt::AlignCenter);  // 文字居中
    waveProgressBar->show();


    // ==================== 最终初始化 ====================
    updateViewTransform();  // 更新视图变换
    view->show();  // 显示游戏视图

    // 调试信息
    qDebug() << "游戏初始化完成";
    qDebug() << "音量滑块位置:" << volumeSlider->geometry();
    qDebug() << "暂停按钮位置:" << pauseResumeButton->geometry();
}

// 音量更新函数 - 将滑块值转换为0.0-1.0范围并设置音量
void game::updateVolume(int value)
{
    qreal volume = value / 100.0;
    mQSound->setVolume(volume);
    mWinSound->setVolume(volume);
    qDebug() << "音量设置为:" << volume;
}

// 视图缩放功能 - 保持游戏画面比例
void game::updateViewTransform()
{
    if (!view) return;

    // 计算缩放比例
    qreal scaleX = static_cast<qreal>(width()) / 900.0;
    qreal scaleY = static_cast<qreal>(height()) / 600.0;
    qreal scaleFactor = qMin(scaleX, scaleY);

    // 应用缩放
    view->resetTransform();
    view->scale(scaleFactor, scaleFactor);

    // 计算居中偏移
    int scaledWidth = 900 * scaleFactor;
    int scaledHeight = 600 * scaleFactor;
    int xOffset = (width() - scaledWidth) / 2;
    int yOffset = (height() - scaledHeight) / 2;

    // 设置视图位置
    view->setGeometry(xOffset, yOffset, scaledWidth, scaledHeight);

    // 更新控件位置（保持右上角相对位置）
    if (pauseResumeButton) {
        pauseResumeButton->move(width() - 110, 10);
    }
    if (volumeSlider) {
        volumeSlider->move(width() - 110, 100);
    }
    if (volumeLabel) {
        volumeLabel->move(width() - 110, 60);
    }

    // 在updateViewTransform()中添加进度条位置更新
    if (waveProgressBar) {
        waveProgressBar->move(width() - 110, 150);  // 与构造函数中的y坐标一致
    }
}

// 窗口大小改变事件 - 重新调整视图
void game::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateViewTransform();
}

// 析构函数 - 释放所有资源
game::~game()
{
    delete mQSound;
    delete mWinSound;
    delete mQTimer;
    delete view;
    delete scene;
    delete finalWaveTimer;
    delete textShowTimer;
    delete textItem;
    // 注意：Qt对象有父对象时会自动释放，不需要手动delete所有子对象
}

// 隐藏纸条并开始游戏引导
void game::hideNoteAndStartGame()
{
    scene->removeItem(noteItem);
    delete noteItem;
    noteItem = nullptr;
    isNoteShowing = false;
    textShowTimer->start();  // 开始显示文字引导图片
}

// 显示下一张引导图片
void game::showNextTextImage()
{
    textShowTimer->stop();

    if (currentTextIndex < textPixmaps.size()) {
        textItem->setPixmap(textPixmaps[currentTextIndex]);
        textItem->show();
        qDebug() << "显示第" << currentTextIndex + 1 << "张引导图片";
        currentTextIndex++;
        textShowTimer->start(1000);
    } else {
        textItem->hide();
        mQSound->play();    // 开始播放背景音乐
        mQTimer->start(33); // 启动游戏主循环（约30FPS）
        qDebug() << "游戏正式开始";
    }
}

// 僵尸生成逻辑（根据波次调整生成频率和类型）
void game::addZombie()
{
    // 如果已触发最后一波，停止常规生成
    if (isFinalWave) return;

    static int counter = 0;
    int currentInterval;

    // ==================== 第一个判断：生成间隔 ====================
    // 根据当前波次确定生成间隔
    if (currentWave <= 5) {
        currentInterval = 15 * 1000 / 33;  // 前5波：15秒
    } else if (currentWave <= 10) {
        currentInterval = qrand() % (2 * 1000 / 33) + (10 * 1000 / 33);  // 6-10波：10-12秒
    } else if (currentWave <= 15) {
        currentInterval = qrand() % (2 * 1000 / 33) + (8 * 1000 / 33);   // 11-15波：8-10秒
    } else {
        currentInterval = qrand() % (1 * 1000 / 33) + (6 * 1000 / 33);   // 16波后：6-7秒
    }

    // 生成僵尸
    if (++counter >= currentInterval) {
        // 特殊处理：第一次生成时显示1（currentWave=1）
        if (currentWave == 1) {
            waveProgressBar->setValue(1);  // 显示"波次: 1/20"
        }

        qDebug() << "生成第" << currentWave << "波僵尸";

        // ==================== 第二个判断：僵尸数量 ====================
        int zombieCountThisWave = 1;
        if (currentWave <= 5) {
            zombieCountThisWave = 1;
        } else if (currentWave <= 10) {
            zombieCountThisWave = 2;
        } else if (currentWave <= 15) {
            zombieCountThisWave = 3;
        } else if (currentWave <= 20) {
            zombieCountThisWave = 5;
        }

        // ========== 第三步：循环生成对应数量的僵尸 ==========
        for (int zomIdx = 0; zomIdx < zombieCountThisWave; zomIdx++)
        {
            int type = qrand() % 100; // 0-99的随机数，用于概率判断
            int row = qrand() % 5;    // 0-4行，随机分配到游戏5行中
            zombie *zom = nullptr;

            // 按波次调整僵尸类型概率（修复中期冗余问题，确保难度递增）
            if (currentWave < 5) {
                // 前期（1-5波）：基础僵尸为主（80%），特殊僵尸少
                if (type < 80)
                    zom = new basiczombie;
                else if (type < 90)
                    zom = new ConeZombie;   // 10%
                else if (type < 95)
                    zom = new BucketZombie; // 5%
                else if (type < 98)
                    zom = new ScreenZombie; // 3%
                else
                    zom = new FootballZombie; // 2%
            } else if (currentWave < 10) {
                // 中期（6-10波）：降低普通僵尸（60%），提升特殊僵尸占比
                if (type < 60)
                    zom = new basiczombie;
                else if (type < 75)
                    zom = new ConeZombie;   // 15%
                else if (type < 88)
                    zom = new BucketZombie; // 13%
                else if (type < 98)
                    zom = new ScreenZombie; // 10%
                else
                    zom = new FootballZombie; // 2%
            } else if (currentWave < 15) {
                // 中后期（11-15波）：普通僵尸进一步减少（40%），高难度僵尸为主
                if (type < 40)
                    zom = new basiczombie;
                else if (type < 55)
                    zom = new ConeZombie;   // 15%
                else if (type < 70)
                    zom = new BucketZombie; // 15%
                else if (type < 90)
                    zom = new ScreenZombie; // 20%
                else
                    zom = new FootballZombie; // 10%
            } else {
                // 后期（16波+）：极少普通僵尸（10%），高难度僵尸占绝对主力
                if (type < 10)
                    zom = new basiczombie;
                else if (type < 20)
                    zom = new ConeZombie;   // 10%
                else if (type < 35)
                    zom = new BucketZombie; // 15%
                else if (type < 75)
                    zom = new ScreenZombie; // 40%
                else
                    zom = new FootballZombie; // 25%
            }
            if (zom != nullptr) {
                zom->setPos(1028, 120 + 95 * row);
                scene->addItem(zom);
            }
        }


        if(currentWave!=20){
        // 先更新进度条显示当前波次
        waveProgressBar->setValue(currentWave);
        }
        // 然后再增加波次（为下一波做准备）
        currentWave++;
        counter = 0;

        // 检查是否触发最后一波
        if (currentWave >= FINAL_WAVE) {
            triggerFinalWave();
        }
    }
}

// 游戏状态检查（胜利/失败条件）
void game::check()
{
    // 检查是否有僵尸到达最左边（游戏失败）
    const QList<QGraphicsItem *> items = scene->items();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == zombie::Type && item->x() < 150) {
            // 停止阳光收集音效
            if (sun::s_collectSound) {
                sun::s_collectSound->stop();
                sun::s_collectCount = 0;  // 重置计数
            }
            // 直接操作僵尸类的静态音效变量停止音效
            if (zombie::s_eatSound) {
                zombie::s_eatSound->stop();
            }
            // 直接操作子弹类的静态音效变量停止音效
            if (peashot::s_hitSound) {
                peashot::s_hitSound->stop();
            }
            scene->addPixmap(QPixmap(":/new/prefix1/ZombiesWon.png"))->setPos(336, 92);
            scene->advance();
            mQTimer->stop();
            return;
        }
    }

    // 检查是否所有僵尸都被消灭（游戏胜利）
    if (isFinalWave && isAllZombiesDead()) {
        showWinScreen();
    }
}

// 隐藏最后一波提示
void game::hideFinalWaveText()
{
    finalWaveTextItem->hide();
    isFinalWaveTextShowing = false;
    qDebug() << "最后一波提示隐藏";
}

// 触发最后一波僵尸
void game::triggerFinalWave()
{
    waveProgressBar->setValue(FINAL_WAVE);  // 确保进度条显示满格

    if (!isFinalWaveTextShowing) {
        finalWaveTextItem->show();
        isFinalWaveTextShowing = true;
        finalWaveTextTimer->start();
        qDebug() << "显示最后一波提示";
    }

    isFinalWave = true;
    qDebug() << "触发最后一波僵尸！";

    // 生成第一批最后一波僵尸
    for (int row = 0; row < 5; ++row) {
        ScreenZombie *finalZom = new ScreenZombie;
        finalZom->speed *= 3;  // 加速
        finalZom->setPos(1028, 120 + 95 * row);
        scene->addItem(finalZom);
    }

    finalWaveBatch = 1;
    finalWaveTimer->start();  // 准备生成第二批
}

// 检查是否所有僵尸都被消灭
bool game::isAllZombiesDead()
{
    const QList<QGraphicsItem *> items = scene->items();
    foreach (QGraphicsItem *item, items) {
        if (item->type() == zombie::Type) {
            return false;
        }
    }
    return true;
}

// 生成第二批最后一波僵尸
void game::generateSecondFinalWave()
{
    if (finalWaveBatch == 1) {
        qDebug() << "触发最后一波僵尸（第二批）！";

        for (int row = 0; row < 5; ++row) {
            FootballZombie *finalZom = new FootballZombie;
            finalZom->setPos(1028, 120 + 95 * row);
            scene->addItem(finalZom);
        }

        finalWaveBatch = 2;
        finalWaveTimer->stop();
    }
}

// 显示胜利画面
void game::showWinScreen()
{
    // 直接操作静态音效变量停止音效
    if (zombie::s_eatSound) {
        zombie::s_eatSound->stop();
    }
    if (peashot::s_hitSound) {
        peashot::s_hitSound->stop();
    }
    // 停止阳光收集音效
    if (sun::s_collectSound) {
        sun::s_collectSound->stop();
        sun::s_collectCount = 0;  // 重置计数
    }
    mQTimer->stop();
    mQSound->stop();
    mWinSound->play();

    QGraphicsPixmapItem *winItem = scene->addPixmap(QPixmap(":/new/prefix1/win.png"));
    winItem->setPos(336, 92);
    scene->advance();

    winCloseTimer->start(4000);  // 4秒后关闭窗口
}

// 胜利后关闭窗口
void game::closeWindowAfterWin()
{
    this->close();
}

// 暂停/继续游戏
void game::togglePauseResume()
{
    if (isPaused) {
        mQTimer->start(33);
        mQSound->play();
        pauseResumeButton->setText("暂停");
    } else {
        mQTimer->stop();
        mQSound->stop();
        pauseResumeButton->setText("继续");
    }
    isPaused = !isPaused;
}
