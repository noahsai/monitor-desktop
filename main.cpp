#include "monitot.h"
#include <QApplication>
//----------------------
//测试monitorwindow用
//#include"monitorwindow.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    monitot w;
    w.show();
    return a.exec();

}
