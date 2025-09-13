#ifndef GAME_H
#define GAME_H
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QSound>
#include <QPushButton>           // 按钮类，用于创建暂停/继续按钮
#include <QDateTime>
#include <QGraphicsTextItem>  // 需包含文本项头文件
#include <QSlider>            // 音量滑块
#include <QLabel>             // 音量标签
// 在game.h的类声明中添加
#include <QProgressBar>  // 进度条头文件
class game : public QWidget
{
    Q_OBJECT
public:


    //新增：游戏结束相关变量
    int currentWave;          // 当前僵尸波次（从1开始）
    bool isFinalWave;         // 是否已触发最后一波僵尸
    const int FINAL_WAVE = 20;// 触发最后一波的波次（可自定义，比如20波后）
    QTimer* winCloseTimer;    // 胜利后延迟关窗的计时器

    // 修改：使用 QSoundEffect 替代 QSound
    QSoundEffect* mQSound;        // 背景音乐
    QSoundEffect* mWinSound;      // 胜利音乐

    // 新增：音量控制相关
    QSlider *volumeSlider;    // 音量控制滑块
    QLabel *volumeLabel;      // 音量标签


    // 新增：最后一波单张提示图片相关
    QPixmap finalWaveTextPixmap;  // 单张提示图片
    QGraphicsPixmapItem* finalWaveTextItem;  // 显示项
    QTimer* finalWaveTextTimer;   // 控制显示时长的计时器
    bool isFinalWaveTextShowing;  // 是否正在显示

    //胜利画面
    QTimer* mQTimer;
    QGraphicsView* view;
    QGraphicsScene* scene;
    QPushButton *pauseResumeButton;  // 暂停/继续游戏的按钮
    bool isPaused;               // 标记游戏是否处于暂停状态

    // 新增：控制最后一波僵尸生成间隔的计时器和批次计数
    QTimer* finalWaveTimer;
    int finalWaveBatch;  // 记录已生成的最后一波僵尸批次（1：第一批；2：第二批）


    QGraphicsPixmapItem *noteItem; // 纸条图片项
    QTimer *noteShowTimer; // 控制纸条显示时长的计时器
    bool isNoteShowing = true; // 标记是否正在显示纸条

    // 新增：图片显示相关
    QList<QPixmap> textPixmaps; // 存储三张文字图片
    QGraphicsPixmapItem* textItem; // 显示文字图片的项
    QTimer* textShowTimer;     // 控制图片切换的计时器
    int currentTextIndex;      // 当前显示的图片索引


    // 新增：胜利相关函数
    void triggerFinalWave();  // 触发最后一波僵尸
    bool isAllZombiesDead();  // 判断所有僵尸是否被消灭
    void showWinScreen();     // 显示胜利画面并延迟关窗
    void updateViewTransform();
    void  resizeEvent(QResizeEvent *event);
    explicit game(QWidget *parent = nullptr);
    ~game() override;
    void addZombie();
    void check();

private:
    QProgressBar *waveProgressBar;  // 波次进度条
private slots:
    // 处理暂停/继续按钮点击的槽函数
    void togglePauseResume();
    void closeWindowAfterWin();// 胜利后延迟关窗的槽函数
    // 新增：生成第二批最后一波僵尸的槽函数
    void generateSecondFinalWave();
    void hideNoteAndStartGame();  //开局生成note
    void showNextTextImage();  // 显示下一张文字图片的槽函数
    void hideFinalWaveText();  // 新增：隐藏最后一波提示
    void updateVolume(int value);  // 更新音量
};

#endif // GAME_H
