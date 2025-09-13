#include "startpage.h"
#include <QIcon>
#include <QPushButton>
#include "game.h"
#include "qapplication.h"
#include <QApplication>
#include <QGraphicsPixmapItem>  // 用于添加图片到图形场景
#include <QDebug>               // 调试用（检查图片是否加载成功）
// 包含音频相关头文件（若未包含）
#include <QSoundEffect>
#include <QUrl> // 用于处理音频资源路径
#include <QLabel>               // 用于显示静态图片
// 新增：视频播放相关头文件
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QBoxLayout>
#include <QtMultimedia/QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QSlider>
#include <QHBoxLayout>
#include <QTime>
#include <QFile>

// 析构函数：释放动态内存（避免内存泄漏）
startpage::~startpage()
{
    delete mpainter_1;       // 释放绘图对象
    delete zombieHandTimer;  // 释放计时器
    delete handScene;        // 释放场景
    delete handView;         // 释放视图
    delete handSound;        // 释放音频对象
    delete bgmSound;  // 新增：释放背景音乐对象
    // 释放帮助功能相关内存
    delete helpBtn;
    delete helpDialog;
    // 新增：释放视频播放相关内存
    delete videoBtn;
    delete videoDialog;
    delete videoPlayer;
    delete videoWidget;
    delete fullscreenBtn;
    delete playPauseBtn;
    delete videoSlider;
    delete timeLabel;
    delete videoTimer;
}

startpage::startpage(QWidget *parent)
    : QWidget{parent}
{
    //按钮这些由qt自动绘制（不需要绘制函数）
    this->resize(900,600);
    this->setWindowTitle("PlantsVsZombies Plus Version 95 to rebuild project");
    this->setWindowIcon(QIcon(":/new/prefix1/WallNut.png"));
    mpainter_1=new QPainter(this);
    // 初始化左上角带背景+文字的标签（无冗余 show()）
    QPixmap cornerPixmap(":/new/prefix1/labelone.png"); // 你的标签背景图
    if (!cornerPixmap.isNull()) {
        QLabel *cornerLabel = new QLabel(this); // 父对象设为 startpage，确保跟随父窗口绘制
        // 1. 设置标签背景（用 pixmap 填充整个标签）
        cornerLabel->setPixmap(cornerPixmap);
        // 2. 固定标签大小 = 背景图大小（避免拉伸/压缩）
        cornerLabel->setFixedSize(cornerPixmap.size());
        // 3. 定位到左上角（x=0, y=0）
        cornerLabel->setGeometry(0, 0, cornerPixmap.width(), cornerPixmap.height());

        //cornerLabel->show();
        // ！关键：无需调用 show()，Qt 会在父窗口绘制完成后自动渲染标签
    }
    else {
        qDebug() << "错误：左上角标签背景图加载失败，检查资源路径！";
    }

    // 新增：初始化背景音乐
    bgmSound = new QSoundEffect(this);
    bgmSound->setSource(QUrl("qrc:/new/prefix2/Grazy.wav"));  // 替换为你的背景音乐路径
    bgmSound->setVolume(0.5);  // 音量稍低，不影响音效
    bgmSound->setLoopCount(QSoundEffect::Infinite);  // 无限循环
    bgmSound->play();  // 进入界面立即播放

    //  初始化音频对象（关键步骤）
    handSound = new QSoundEffect(this);
    // 设置音频文件路径（从资源文件中读取，格式：qrc:/资源前缀/音频文件名）
    // 注意：替换 "hand_sound.wav" 为你的 WAV 文件名，确保路径与 .qrc 一致
    handSound->setSource(QUrl("qrc:/new/prefix2/laugh.wav"));
    // 设置音量（0.0~1.0，1.0为最大音量）
    handSound->setVolume(0.7);


    // 初始化僵尸大手组件（场景+视图），设置固定位置
    handScene = new QGraphicsScene(this);
    handView = new QGraphicsView(handScene, this);
    // 设置视图固定位置和大小，根据大手图片尺寸调整
    handView->setGeometry(180, 320, 160, 256);
    // 设置视图样式为无边框、透明背景，使大手更好地与背景融合
    handView->setStyleSheet(R"(
        QGraphicsView {
            border: none;
            background: transparent;
        }
    )");
    // 隐藏水平和垂直滚动条
    handView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    handView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    handView->hide();  // 初始隐藏

    // 计时器：9秒后跳转（可调整）
    zombieHandTimer = new QTimer(this);
    zombieHandTimer->setSingleShot(true);
    zombieHandTimer->setInterval(6000);



    QPushButton* btn_1=new QPushButton(this);
    QPushButton* btn_2=new QPushButton(this);
    QPushButton* btn_3=new QPushButton(this);
    QPushButton* btn_4=new QPushButton(this);
    QPushButton *exitBtn = new QPushButton(this);  //退出按钮

    btn_1->setGeometry(450,60,340,120);
    btn_1->setStyleSheet("QPushButton{"
                         "border:none;" // 设置无边框
                         "padding:0px;" // 设置无内边距
                         "background:transparent;" // 设置背景透明
                         "border-image:url(:/new/prefix1/mx1.png);" // 设置按钮样式，包括背景图片
                         "}"
                         "QPushButton:hover{"
                         "border-image:url(:/new/prefix1/mx.png);" // 鼠标悬停样式
                         "}"
                         );
    btn_2->setGeometry(450,150,320,130);
    btn_2->setStyleSheet("QPushButton{"
                         "border:none;"
                         "padding:0px;"
                         "background:transparent;"
                         "border-image:url(:/new/prefix1/play1.png);"
                         "}"
                         "QPushButton:hover{"
                         "border-image:url(:/new/prefix1/play.png);"
                         "}"
                         );
    btn_3->setGeometry(460,230,290,130);
    btn_3->setStyleSheet("QPushButton{"
                         "border:none;"
                         "padding:0px;"
                         "background:transparent;"
                         "border-image:url(:/new/prefix1/yizi1.png);"
                         "}"
                         "QPushButton:hover{"
                         "border-image:url(:/new/prefix1/yizi.png);"
                         "}"
                         );
    btn_4->setGeometry(460,320,280,120);
    btn_4->setStyleSheet("QPushButton{"
                         "border:none;"
                         "padding:0px;"
                         "background:transparent;"
                         "border-image:url(:/new/prefix1/mini1.png);"
                         "}"
                         "QPushButton:hover{"
                         "border-image:url(:/new/prefix1/mini.png);"
                         "}"
                         );

    exitBtn->setGeometry(808, 520, 58, 21);
    // 样式表设置：透明背景、无边框、匹配字体大小和颜色
    exitBtn->setStyleSheet(
        "QPushButton {"
        "background-color: transparent;"  // 透明背景，完全显示背景贴图
        "border: none;"                   // 去掉边框，不遮挡贴图

        "}"
        "QPushButton:hover {"
        "background-color: rgba(0, 0, 0, 0.15);"  // 悬停时显示微黑色半透明效果
        "}"
        "QPushButton:pressed {"
        "background-color: rgba(0, 0, 0, 0.3);"   // 点击时黑色效果更明显一些
        "}"
        );
    // 连接退出信号
    connect(exitBtn, &QPushButton::clicked, QApplication::quit);

    // -------------------------- 帮助弹窗实现（仅背景+系统关闭按钮） --------------------------
    helpDialog = new QDialog(this);
    // 弹窗基础设置（保留系统标题栏和×按钮）
    helpDialog->setFixedSize(600, 400); // 固定尺寸，比主界面小
    helpDialog->setWindowTitle("游戏帮助"); // 弹窗标题（标题栏会显示×）
    helpDialog->setWindowIcon(QIcon(":/new/prefix1/WallNut.png")); // 弹窗图标
    helpDialog->setModal(true); // 模态：弹窗显示时主界面不可操作
    // 无需额外设置关闭按钮，Qt默认标题栏带×（点击×自动关闭弹窗）

    // 弹窗背景（仅背景图，无其他元素）
    QPixmap helpBg(":/new/prefix1/help.png"); // 替换为你的帮助背景图路径
    if (!helpBg.isNull()) {
        QLabel* bgLabel = new QLabel(helpDialog);
        // 背景图自适应弹窗（保持比例，避免拉伸）
        bgLabel->setPixmap(helpBg.scaled(
            helpDialog->width(),
            helpDialog->height(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
        bgLabel->setGeometry(0, 0, helpDialog->width(), helpDialog->height());
        bgLabel->lower(); // 确保背景在最底层
    } else {
        // 背景图加载失败时，用纯色兜底
        helpDialog->setStyleSheet("background-color: #F0E6D2;");
        qDebug() << "错误：帮助弹窗背景图加载失败！";
    }


    // 连接按钮点击事件，启动游戏窗口，并隐藏当前窗口
    // connect(btn_1,&QPushButton::clicked,[this](){
    //   game* ga=new game;
    //    ga->show();
    //    this->hide();
    // });

    // -------------------------- 帮助按钮（退出按钮左侧） --------------------------
    helpBtn = new QPushButton(this);
    // 位置：退出按钮（x=808, y=520）左侧，x左移70（按钮宽60+间距10），y与退出按钮一致
    helpBtn->setGeometry(728, 528, 60, 21);
    // 样式：与退出按钮统一，保持界面风格
    helpBtn->setStyleSheet(
        "QPushButton{"
        "background-color: transparent;"
        "border: none;"
        "color: #333333;"
        "font-size: 12px;"
        "}"
        "QPushButton:hover{"
        "background-color: rgba(0, 0, 0, 0.15);"
        "}"
        "QPushButton:pressed{"
        "background-color: rgba(0, 0, 0, 0.3);"
        "}"
        );

    // 帮助按钮点击事件：显示弹窗（关闭靠标题栏×）
    connect(helpBtn, &QPushButton::clicked, [this]() {
        helpDialog->show();
    });


    // 按钮触发大手
    connect(btn_1, &QPushButton::clicked, [this]() {
        // 1. 停止背景音乐
        if (bgmSound->isPlaying()) {
            bgmSound->stop();
        }

        // 加载僵尸大手图片
        QPixmap handPixmap(":/new/prefix1/handlast.png");
        if (handPixmap.isNull()) {
            qDebug() << "错误：僵尸大手图片加载失败，请检查路径！";
            return;
        }

        // 按视图大小缩放大手（确保图片填满视图，无偏移）
        QPixmap scaledHand = handPixmap.scaled(
            handView->width(),  // 用视图宽度作为缩放宽度
            handView->height(), // 用视图高度作为缩放高度
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );

        // 场景中图片放在原点（0,0），与视图左上角对齐
        handScene->clear();
        QGraphicsPixmapItem* handItem = handScene->addPixmap(scaledHand);
        handItem->setPos(0, 0);  // 与视图左上角对齐，无偏移

        // 先停止之前可能未播完的音频（避免重复播放）
        if (handSound->isPlaying()) {
            handSound->stop();
        }

        // 开始播放音频（与大手显示同步）
        handSound->play();

        // 显示大手+启动计时器
        handView->show();
        zombieHandTimer->start();

        // 计时结束：隐藏大手→跳转游戏
        connect(zombieHandTimer, &QTimer::timeout, [this]() {
            // 停止音频（即使音频未播完，也强制停止）
            handSound->stop();
            handView->hide();
            game* ga = new game;
            ga->show();
            this->hide();
        });
    });


    // -------------------------- 视频播放功能实现 --------------------------
    // 初始化视频播放按钮
    videoBtn = new QPushButton(this);
    // 位置：帮助按钮左侧
    videoBtn->setGeometry(650, 490, 70, 30);
    videoBtn->setStyleSheet(
        "QPushButton{"
        "background-color: transparent;"
        "border: none;"
        "color: #333333;"
        "font-size: 12px;"
        "}"
        "QPushButton:hover{"
        "background-color: rgba(0, 0, 0, 0.15);"
        "}"
        "QPushButton:pressed{"
        "background-color: rgba(0, 0, 0, 0.3);"
        "}"
        );

    // 初始化视频播放对话框
    videoDialog = new QDialog(this);
    videoDialog->setWindowTitle("植物大战僵尸主题曲");
    videoDialog->setFixedSize(800, 500);
    videoDialog->setModal(false);
    // 设置关闭时完全关闭而不是隐藏
    videoDialog->setAttribute(Qt::WA_DeleteOnClose, false);

    // 重写关闭事件，确保正确关闭视频
    videoDialog->installEventFilter(this);

    // 初始化视频播放器
    videoPlayer = new QMediaPlayer(this);
    videoWidget = new QVideoWidget(videoDialog);

    // 设置视频播放器输出到视频部件
    videoPlayer->setVideoOutput(videoWidget);

    // 设置视频源
    videoPlayer->setMedia(QUrl("qrc:/new/prefix2/theme.mp4"));

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(videoDialog);

    // 添加视频部件
    mainLayout->addWidget(videoWidget);

    // 创建控制栏布局
    QHBoxLayout *controlLayout = new QHBoxLayout();

    // 初始化播放/暂停按钮
    playPauseBtn = new QPushButton("暂停", videoDialog);
    playPauseBtn->setFixedSize(60, 25);

    // 初始化进度条
    videoSlider = new QSlider(Qt::Horizontal, videoDialog);
    videoSlider->setRange(0, 0);

    // 初始化时间标签
    timeLabel = new QLabel("00:00 / 00:00", videoDialog);
    timeLabel->setFixedSize(100, 25);

    // 初始化全屏按钮
    fullscreenBtn = new QPushButton("全屏", videoDialog);
    fullscreenBtn->setFixedSize(60, 25);

    // 添加关闭按钮
    QPushButton *closeBtn = new QPushButton("关闭", videoDialog);
    closeBtn->setFixedSize(60, 25);

    // 添加控件到控制栏
    controlLayout->addWidget(playPauseBtn);
    controlLayout->addWidget(videoSlider);
    controlLayout->addWidget(timeLabel);
    controlLayout->addWidget(fullscreenBtn);
    controlLayout->addWidget(closeBtn);

    // 添加控制栏到主布局
    mainLayout->addLayout(controlLayout);

    // 初始化视频计时器
    videoTimer = new QTimer(this);
    videoTimer->setInterval(100);

    // 连接信号和槽
    connect(videoBtn, &QPushButton::clicked, this, &startpage::toggleVideoPlayback);
    connect(fullscreenBtn, &QPushButton::clicked, this, &startpage::toggleFullscreen);
    connect(closeBtn, &QPushButton::clicked, this, &startpage::closeVideoDialog);
    connect(playPauseBtn, &QPushButton::clicked, [this]() {
        if (videoPlayer->state() == QMediaPlayer::PlayingState) {
            videoPlayer->pause();
            playPauseBtn->setText("播放");
        } else {
            // 确保视频在播放前处于正确状态
            if (videoPlayer->state() == QMediaPlayer::StoppedState) {
                videoPlayer->setPosition(0);
            }
            videoPlayer->play();
            playPauseBtn->setText("暂停");
        }
    });

    connect(videoSlider, &QSlider::sliderMoved, this, &startpage::setVideoPosition);
    connect(videoTimer, &QTimer::timeout, this, &startpage::updateVideoPosition);
    connect(videoPlayer, &QMediaPlayer::durationChanged, this, &startpage::updateDurationInfo);
    connect(videoPlayer, &QMediaPlayer::stateChanged, this, &startpage::handleVideoStateChanged);
    connect(videoDialog, &QDialog::finished, this, &startpage::closeVideoDialog);

    // 设置视频渲染质量
    videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
}

// 切换视频播放状态
void startpage::toggleVideoPlayback()
{
    if (videoDialog->isVisible()) {
        closeVideoDialog();
    } else {
        // 暂停背景音乐
        if (bgmSound->isPlaying()) {
            bgmSound->stop();
        }

        // 确保视频从开始位置播放
        if (videoPlayer->state() == QMediaPlayer::StoppedState) {
            videoPlayer->setPosition(0);
        }

        // 确保对话框处于正常状态（非全屏）
        if (videoDialog->isFullScreen()) {
            videoDialog->showNormal();
            fullscreenBtn->setText("全屏");
        }

        videoDialog->show();
        videoPlayer->play();
        videoTimer->start();
        playPauseBtn->setText("暂停");

        // 确保对话框获得焦点
        videoDialog->activateWindow();
        videoDialog->raise();
    }
}

// 切换全屏状态
void startpage::toggleFullscreen()
{
    if (videoDialog->isFullScreen()) {
        videoDialog->showNormal();
        fullscreenBtn->setText("全屏");
    } else {
        videoDialog->showFullScreen();
        fullscreenBtn->setText("退出全屏");
    }
}

// 更新视频位置
void startpage::updateVideoPosition()
{
    if (videoPlayer->state() == QMediaPlayer::StoppedState) {
        return;
    }

    qint64 position = videoPlayer->position();
    qint64 duration = videoPlayer->duration();

    // 更新进度条
    videoSlider->setRange(0, duration);
    videoSlider->setValue(position);

    // 更新时间显示
    QTime currentTime(0, 0, 0, 0);
    currentTime = currentTime.addMSecs(position);
    QTime totalTime(0, 0, 0, 0);
    totalTime = totalTime.addMSecs(duration);

    QString format = "mm:ss";
    if (duration > 3600000) {
        format = "hh:mm:ss";
    }

    timeLabel->setText(currentTime.toString(format) + " / " + totalTime.toString(format));
}

// 设置视频位置
void startpage::setVideoPosition(int position)
{
    // 防止在设置位置时视频状态不一致
    QMediaPlayer::State oldState = videoPlayer->state();
    videoPlayer->setPosition(position);

    // 如果之前是播放状态，继续播放
    if (oldState == QMediaPlayer::PlayingState) {
        videoPlayer->play();
    }
}

// 更新时长信息
void startpage::updateDurationInfo(qint64 duration)
{
    videoSlider->setRange(0, duration);
}

// 处理视频状态变化
void startpage::handleVideoStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
        videoTimer->stop();
        playPauseBtn->setText("播放");

        // 视频播放完毕后，确保对话框回到正常状态
        if (videoDialog->isFullScreen()) {
            videoDialog->showNormal();
            fullscreenBtn->setText("全屏");
        }

        // 延迟一点时间再关闭对话框，让用户看到播放完毕的状态
        QTimer::singleShot(1000, this, [this]() {
            if (videoPlayer->state() == QMediaPlayer::StoppedState) {
                closeVideoDialog();
            }
        });
    } else if (state == QMediaPlayer::PlayingState) {
        videoTimer->start();
        playPauseBtn->setText("暂停");
    } else if (state == QMediaPlayer::PausedState) {
        videoTimer->stop();
        playPauseBtn->setText("播放");
    }
}

// 关闭视频对话框
void startpage::closeVideoDialog()
{
    // 停止视频播放
    videoPlayer->stop();
    videoTimer->stop();

    // 确保对话框回到正常状态
    if (videoDialog->isFullScreen()) {
        videoDialog->showNormal();
        fullscreenBtn->setText("全屏");
    }

    // 隐藏对话框
    videoDialog->hide();

    // 恢复背景音乐
    if (!bgmSound->isPlaying()) {
        bgmSound->play();
    }
}

// 事件过滤器，处理对话框关闭事件
bool startpage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == videoDialog && event->type() == QEvent::Close) {
        closeVideoDialog();
        return true; // 事件已处理
    }
    return QWidget::eventFilter(obj, event);
}

// 绘制背景
void startpage::paintEvent(QPaintEvent *event)
{
    mpainter_1->begin(this);
    QImage img(":/new/prefix1/Surface.png");
    mpainter_1->drawImage(QRect(0, 0, 900, 600), img);
    mpainter_1->end();
}
