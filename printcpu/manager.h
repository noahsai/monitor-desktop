#ifndef manager_H
#define manager_H

#include <QWidget>
#include "QTimer"
#include "QFile"
#include "QTextStream"
#include"QDebug"
#include"QRegularExpression"
#include<QScrollArea>
#include"printcpu/widget.h"
#include<QtMath>
#include<QSettings>
#include<QMouseEvent>
#include<QDesktopWidget>

namespace Ui {
class manager;
}

class manager : public QScrollArea
{
    Q_OBJECT

public:
    explicit manager(QScrollArea *parent = 0);
    ~manager();

private slots:
    void timeout();

signals:
    void cpuchange(QString&);
private:
    Ui::manager *ui;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void wheelEvent(QWheelEvent *);
    QList<float>  getresult(QStringList&,QStringList&);
    float colculate(QString& one,QString& two);void initui();
    void getcpu();
    void refresh();
    void smallrefresh();
    void saveset();
    void readset();
    void setalpha(int );
    QTimer *timer;
    QFile file;
    QStringList list;
    QStringList list2;
    QRegularExpression reg;
    QRegularExpressionMatchIterator matchs;
    bool isone;
    int row;
    int col;
    int time;
    int times_1s;
    int alpha;
    QList<float>  result;
    QList<Widget*> cpulist;
    QPoint oldpos;
};

#endif // manager_H
