#include "medeawindow.h"

#include <QApplication>
#include <QDebug>

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

    w->show();
    return a.exec();
}
