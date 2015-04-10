#include <QApplication>
#include <QtDebug>
#include <QObject>

#include "medeawindow.h"
#include "modeltester.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Defence Information Group");
    QCoreApplication::setApplicationName("MEDEA");


    QString GraphMLFile = 0;
    if(argc == 2){
        GraphMLFile = QString(argv[1]);
    }

    MedeaWindow *w = new MedeaWindow(GraphMLFile);
    w->show();
    w->setupDefaultSettings();

    return a.exec();
}
