#include "mainwindow.h"
#include <QApplication>
#define m(x, y) (x)<(y)?x:y
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int **b[5][6];
    int *c = 0;
    int pSize = sizeof(b);
    int pSizec = sizeof(c);
    int m = 10*(10)<(15)?(10):(15);

    MainWindow w;
    w.show();

    return a.exec();
}
