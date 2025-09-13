#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
// 包含音频类头文件
#include <QSoundEffect>
// 新增：包含弹窗所需头文件
#include <QDialog>
#include <QLabel>
#include <QPushButton>
// 新增：视频播放相关头文件
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QBoxLayout>
#include <QtMultimedia/QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QSlider>
#include <QHBoxLayout>

class startpage : public QWidget
{
    Q_OBJECT
public:
    QPainter* mpainter_1;
    QTimer* zombieHandTimer;            // 控制大手显示时长的计时器
    QGraphicsScene* handScene;          // 承载大手图片的场景
    QGraphicsView* handView;            // 显示大手的视图
    // 2. 声明音频对象指针（用于播放WAV）
    QSoundEffect* handSound;            // 大手出现的音效
    QSoundEffect* bgmSound;             // 新增：背景音乐（循环播放）
    // 帮助功能相关声明（仅指针）
    QPushButton* helpBtn;       // 帮助按钮
    QDialog* helpDialog;        // 帮助弹窗（依赖系统关闭按钮）
    // 新增：视频播放相关声明
    QPushButton* videoBtn;      // 视频播放按钮
    QDialog* videoDialog;       // 视频播放对话框
    QMediaPlayer* videoPlayer;  // 视频播放器
    QVideoWidget* videoWidget;  // 视频显示部件
    QPushButton* fullscreenBtn; // 全屏按钮
    QPushButton* playPauseBtn;  // 播放/暂停按钮
    QSlider* videoSlider;       // 视频进度条
    QLabel* timeLabel;          // 时间显示标签
    QTimer* videoTimer;         // 更新视频进度的计时器

    void paintEvent(QPaintEvent *event);
    explicit startpage(QWidget *parent = nullptr);
    //  声明析构函数（释放音频对象内存）
    ~startpage() ;
    // 新增：视频播放相关槽函数
private slots:
    void toggleVideoPlayback();
    void toggleFullscreen();
    void updateVideoPosition();
    void setVideoPosition(int position);
    void updateDurationInfo(qint64 duration);
    void handleVideoStateChanged(QMediaPlayer::State state);
    void closeVideoDialog();
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // STARTPAGE_H
