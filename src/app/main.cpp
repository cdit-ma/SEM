#include <QApplication>
#include <QString>

#include "Controllers/ViewController/viewcontroller.h"
#include "Controllers/WindowManager/windowmanager.h"
#include "Widgets/Windows/mainwindow.h"
#include "Controllers/SettingsController/settingscontroller.h"

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
    
    auto window = WindowManager::manager()->constructMainWindow(view_controller);
    if (argc == 2) {
        QString projectPath = QString::fromUtf8(argv[1]);
        if(!projectPath.isEmpty()){
            view_controller->OpenExistingProject(projectPath);
        }
    }
    a.setActiveWindow(window);

    auto result = a.exec();

    //Teardown singletons
    SettingsController::teardownSettings();
    Theme::teardownTheme();
    return result;
}

int main(int argc, char *argv[])
{
    launchMEDEA(argc, argv);
}
