#include <QApplication>
#include <QtDebug>
#include <QObject>

#include <QFont>
#include "Widgets/medeawindow.h"
#include "Widgets/New/medeamainwindow.h"
#include "Widgets/New/medeawindowmanager.h"
#include "Controller/viewcontroller.h"
#include "modeltester.h"

#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QProcess>

#include <QList>
#include <QPair>
#include <QApplication>
#include <string>
#include "View/theme.h"

void testModel(){
    ModelTester *t = new ModelTester();
    t->loadTest("/home/dig/Desktop/HelloWorld(2).graphml");
    delete t;
}

int launchMEDEA(int argc, char *argv[]){
    //Construct a QApplication
    QApplication a(argc, argv);

    //Fixes MacOS QIcon resolution.
    a.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QString graphmlFilePath = "";
    if (argc == 2) {
        graphmlFilePath = QString::fromUtf8(argv[1]);
    }

    ViewController* vc = new ViewController();

    MedeaMainWindow* w2 = (MedeaMainWindow*) MedeaWindowManager::constructMainWindow();
    w2->setViewController(vc);

    MedeaWindow *w = new MedeaWindow(vc, graphmlFilePath);
    a.setActiveWindow(w2);
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
