#include "monitot.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    monitot w;
    w.show();

    return a.exec();
}
