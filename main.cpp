#include <QApplication>
#include <QtDebug>

#include "medeawindow.h"
#include "mainwindow.h"
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MedeaWindow w;
    //MainWindow w2;

    w.show();
    //w2.show();

    return a.exec();
}
