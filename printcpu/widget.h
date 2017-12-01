#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"QPainter"
#include"QTimer"
#include"QPainterPath"
#include"QDebug"
#include"QPixmap"
#include"QLabel"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    void adddata(float pre);

    void set1s_times(int times);
    void timechange( int time);
    void setcpunum(int n);
    ~Widget();

private     slots:
   // void timeout();

private:
    QSize sizeHint();
    void paintEvent(QPaintEvent *event);
    Ui::Widget *ui;
    void initlist();
    qreal lms(int alls , int prems);
    qreal getx(int ms);
    qreal gety(float  per);
    int posx;
    int posy;
    QList <float>  datalist;
    //QTimer timer;
    int times_1s,time;
    QPixmap *pix;
    int show_second;
    int howlong_fresh;
    int cpunum;
    //int now;
};

#endif // WIDGET_H
