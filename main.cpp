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

    ViewController* vc = new ViewController();
    MainWindow* window = (MainWindow*) WindowManager::constructMainWindow(vc);

    if (argc == 2) {
        QString projectPath = QString::fromUtf8(argv[1]);
        if(!projectPath.isEmpty()){
            vc->openExistingProject(projectPath);
        }
    }

    a.setActiveWindow(window);
    return a.exec();
}

int main(int argc, char *argv[])
{
    launchMEDEA(argc, argv);
}
