#include <QApplication>
#include <QtDebug>
#include <QObject>

#include <QFont>
#include "medeawindow.h"
#include "modeltester.h"

#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QProcess>

#include <QList>
#include <QPair>
#include <string>

//ICON_LINK
//https://www.google.com/design/icons/index.html
void testModel(){
    ModelTester *t = new ModelTester();
    t->loadTest("E:/MCMS.graphml");
    delete t;
}

int launchMEDEA(int argc, char *argv[]){
    //Construct a QApplication
    QApplication a(argc, argv);
    a.setQuitLockEnabled(true);

    QString graphmlFilePath = "";
    if (argc == 2) {
        graphmlFilePath = QString(argv[1]);
    }

    MedeaWindow *w = new MedeaWindow(graphmlFilePath);
    a.setActiveWindow(w);

    return a.exec();
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    launchMEDEA(argc, argv);
    //testModel();
}
