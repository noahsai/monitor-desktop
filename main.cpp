#include "monitot.h"
#include <QApplication>
#include"monitorwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    monitot w;
    //monitorwindow w;
    w.show();

    return a.exec();
}
