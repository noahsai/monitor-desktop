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

#define RED  ":/red.png"
#define YELLOW  ":/yellow.png"
#define GREEN  ":/green.png"

namespace Ui {
class monitot;
}

class monitot : public QWidget
{
    Q_OBJECT

public:
    explicit monitot(QWidget *parent = 0);
    ~monitot();

private:
    Ui::monitot *ui;
    void paintEvent(QPaintEvent *);
    int memused;
    QString upspeed;
    QString downspeed;

private slots:
    void timeout();
    void openmoniter();
private:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void contextMenuEvent(QContextMenuEvent *);

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
    QAction *tuichu;
    QAction *moniter;
};

#endif // MONITOT_H
