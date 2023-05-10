#include "UARTGui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UARTGui w;
    w.show();
    return a.exec();
}
