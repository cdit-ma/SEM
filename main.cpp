#include <QApplication>
#include <QtDebug>
#include <QObject>
#include <QFont>
#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QProcess>
#include <QList>
#include <QPair>
#include <QApplication>
#include <string>

#include "Controllers/ViewController/viewcontroller.h"
#include "Controllers/WindowManager/windowmanager.h"
#include "Widgets/Windows/mainwindow.h"


// taskkill
// /F /fi "IMAGENAME eq medea.exe"

int launchMEDEA(int argc, char *argv[]){
    //Construct a QApplication
    QApplication a(argc, argv);
    //Fixes MacOS QIcon resolution.
    a.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    QStringList arguments = a.arguments();

    ViewController* vc = new ViewController();
    MainWindow* window = (MainWindow*) WindowManager::constructMainWindow(vc);


    if (arguments.length() == 2) {
        vc->openExistingProject(arguments.at(1));
    }

    a.setActiveWindow(window);
    return a.exec();
}

int main(int argc, char *argv[])
{
    launchMEDEA(argc, argv);
}
