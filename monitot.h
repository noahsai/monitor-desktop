#ifndef MONITOT_H
#define MONITOT_H

#include <QWidget>
#include <QPainter>
//#include <QPoint>
#include <QTimer>
#include<QMouseEvent>
#include<QDesktopWidget>
#include<QProcess>
#include<QRegularExpression>
#include<QDebug>
#include<QMenu>
#include<QPropertyAnimation>
#include<QSettings>
#include"printcpu/manager.h"



#define RED  ":/red.png"
#define YELLOW  ":/yellow.png"
#define GREEN  ":/green.png"

class monitorwindow;//在cpp文件里包含h文件

namespace Ui {
class monitot;
}

class monitot : public QWidget
{
    Q_OBJECT

public:
    explicit monitot(QWidget *parent = 0);
    int memusing();
    ~monitot();

private:
    Ui::monitot *ui;
    void paintEvent(QPaintEvent *);
    int memused;
    QString upspeed;
    QString downspeed;


private slots:
    void animationfinished();
    void timeout();
    void openmoniter();
    void opencpu();
private:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void contextMenuEvent(QContextMenuEvent *);

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    int side;//左0,上1,右2，非贴边-1；
    bool showall;

    void saveset();
    void readset();


    void getnetspeed();//可独立使用，加返回值即可
    void getmemused();//同上
    void poscheck();

    QProcess *pro;
    QTimer *timer;
    QRegularExpression reg;
    qint64 oup,odown;
    QPoint oldpos;
    bool mousepressed;
//    QSystemTrayIcon *trayIcon;//托盘图标
    QMenu *menu;
//    QAction *tuichu;
//    QAction *moniter;

    monitorwindow *killer;
    bool justpress;
    QPropertyAnimation *animation;
    manager *cpumonitor;
};

#endif // MONITOT_H
