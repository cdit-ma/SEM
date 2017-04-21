#include <QApplication>
#include <QString>

#include "Controllers/ViewController/viewcontroller.h"
#include "Controllers/WindowManager/windowmanager.h"
#include "Widgets/Windows/mainwindow.h"
#include "Controllers/SettingsController/settingscontroller.h"

// taskkill
// /F /fi "IMAGENAME eq medea.exe"

int launchMEDEA(int argc, char *argv[]){
    //Construct a QApplication
    QApplication a(argc, argv);

    //Initialize images
    Q_INIT_RESOURCE(images);
    Q_INIT_RESOURCE(workers);

    //Fixes MacOS QIcon resolution.
    a.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    //Construct a SettingsController and ViewController
    auto settings_controller = SettingsController::settings();
    auto view_controller = new ViewController();

    auto window = WindowManager::constructMainWindow(view_controller);

    if (argc == 2) {
        QString projectPath = QString::fromUtf8(argv[1]);
        if(!projectPath.isEmpty()){
            view_controller->openExistingProject(projectPath);
        }
    }
    a.setActiveWindow(window);
    return a.exec();
}

int main(int argc, char *argv[])
{
    launchMEDEA(argc, argv);
}
