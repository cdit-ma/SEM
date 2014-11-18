#include <QApplication>
#include <QtDebug>

#include "newmedeawindow.h"
//#include "medeawindow.h"
//#include "mainwindow.h"
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MedeaWindow w;
    //MainWindow w2;
    NewMedeaWindow w3;

    w3.show();
    //w2.show();
    //w3.show();

    return a.exec();
}
