#include "monitot.h"
#include <QApplication>
//----------------------
//测试monitorwindow用
//#include"monitorwindow.h"
//----------------------

//-------------------
//deepin下编译用，非dtk环境下注释掉即可
#include<QObject>
#include <DApplication>
#include <QProcess>
#include <QString>
DWIDGET_USE_NAMESPACE
//-------------------

int main(int argc, char *argv[])
{
    QProcess pro;
    pro.start("uname -a");
    if(pro.waitForFinished(3000))
    {
        QString system = pro.readAllStandardOutput();
        if(system.indexOf("eepin")!=-1)
        {
            qDebug()<<"deepin下支持只打开一个此软件";
            DApplication app(argc, argv);
            monitot w;
            if (app.setSingleInstance("noahsai-monitor-desktop")) {
                QObject::connect(&app,&DApplication::newInstanceStarted,&w,&monitot::show);
                w.show();
                return app.exec();
            }
            return 0;
        }
        else {
            //其他系统不能之打开一个实例，主要是我自己写的‘只运行一个实例’的功能有问题，所以该功能直接用dtk的了
                QApplication a(argc, argv);
                monitot w;
                w.show();
                return a.exec();
        }
    }
    return 0;

//非dtk环境下编译使用
//    QApplication a(argc, argv);
//    monitot w;
//    w.show();
//    return a.exec();

}
