#include "QtWidgets/QApplication"
#include "cardgame.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CardGame w;
    w.show();

    return a.exec();
}
