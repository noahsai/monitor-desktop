#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QWidget>
#include <QTimer>
#include<QDesktopWidget>
#include<QProcess>
#include<QRegularExpression>
#include<QDebug>
#include<QTableWidgetItem>
#include<QMouseEvent>

#define CPU_SORT "ps ax -o \"%u %p %C\" -o pmem -o \"%c %a\" --sort=-%cpu|head -16"//包含头
#define MEM_SORT "ps ax -o \"%u %p %C\" -o pmem -o \"%c %a\" --sort=-pmem|head -16"//包含头
#define BG_color "#monitorwindow{ background-color:%1; }\
                #pushButton{ color:%2; }"
#define LOW "rgb(50, 162, 106)"
#define MID "rgb(233, 119, 57)"
#define HIGH "rgb(179, 10, 13)"

class monitot;//在cpp文件里包含h文件

namespace Ui {
class monitorwindow;
}

class monitorwindow : public QWidget
{
    Q_OBJECT

public:
    explicit monitorwindow(QWidget *parent = 0);
    void stop_hide();
    ~monitorwindow();

public slots:

    bool getinfo();
    bool cpuchange(QString&);
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_tableWidget_itemSelectionChanged();


    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

private:
    Ui::monitorwindow *ui;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    QPoint oldpos;
    void makelist();
    bool pidselected(const QString&);
    QTimer *timer;
    QProcess *process;
    QStringList selectedpid;
    QString cmd;
    QString pressed;
    QStringList selected;
    QStringList listupdate2freshselected;
};

#endif // MONITORWINDOW_H
