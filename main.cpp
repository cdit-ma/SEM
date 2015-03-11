#include <QApplication>
#include <QtDebug>
#include <QObject>

#include "medeawindow.h"
#include "modeltester.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString GraphMLFile = 0;
    if(argc == 2){
        //Got File name;;
        qCritical() << "Got FIle";
        GraphMLFile = QString(argv[1]);
        qCritical() << QString(argv[0]);
        qCritical() << GraphMLFile;
    }

    MedeaWindow *w = new MedeaWindow(GraphMLFile);

    //ModelTester *t = new ModelTester();
    //delete t;
    //return 0;

    w->show();
    w->setupViewLayout();

    return a.exec();
}
