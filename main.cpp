#include <QApplication>
#include <QtDebug>
#include <QObject>

#include <QFont>
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

// taskkill
// /F /fi "IMAGENAME eq medea.exe"
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

    ViewController* vc = new ViewController();
    MedeaMainWindow* window = (MedeaMainWindow*) MedeaWindowManager::constructMainWindow(vc);

    if (argc == 2) {
        QString projectPath = QString::fromUtf8(argv[1]);
        if(!projectPath.isEmpty()){
            vc->openExistingProject(projectPath);
        }
    }

    a.setActiveWindow(window);
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
