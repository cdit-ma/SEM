#include <QApplication>
#include <QtDebug>

#include "medeawindow.h"
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MedeaWindow w;
    w.show();
    return a.exec();
}
