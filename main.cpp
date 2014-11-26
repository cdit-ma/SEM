#include "medeawindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MedeaWindow w;

    w.show();
    return a.exec();
}
