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
    QString graphmlFilePath = "";
    if (argc == 2) {
        graphmlFilePath = QString::fromUtf8(argv[1]);
    }


    Theme::theme()->setDefaultImageTintColor(QColor(70,70,70));
    Theme::theme()->setIconToggledImage("Actions", "Grid_On", "Actions", "Grid_Off");
    Theme::theme()->setIconToggledImage("Actions", "Fullscreen", "Actions", "Failure");
    Theme::theme()->setIconToggledImage("Actions", "Minimap", "Actions", "Invisible");
    Theme::theme()->setIconToggledImage("Actions", "Arrow_Down", "Actions", "Arrow_Up");
    Theme::theme()->setIconToggledImage("Actions", "SearchOptions", "Actions", "Arrow_Down");
    Theme::theme()->setIconToggledImage("Actions", "DockMaximize", "Actions", "Minimize");
    Theme::theme()->setIconToggledImage("Actions", "Lock_Open", "Actions", "Lock_Closed");
    Theme::theme()->setIconToggledImage("Actions", "Invisible", "Actions", "Visible");

    //LOAD THINGS
    //emit Theme::theme()->initPreloadImages();
    //emit Theme::theme()->theme_Changed();
    Theme::theme()->preloadImages();

    ViewController* vc = new ViewController();
    MedeaMainWindow* w2 = (MedeaMainWindow*) MedeaWindowManager::constructMainWindow(vc);
    emit Theme::theme()->theme_Changed();
    MedeaWindow *w = new MedeaWindow(vc, ":/demo.graphml");



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
