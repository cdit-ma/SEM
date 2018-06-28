#include <QApplication>
#include <QString>

#include "Controllers/ViewController/viewcontroller.h"
#include "Controllers/WindowManager/windowmanager.h"
#include "Widgets/Windows/mainwindow.h"
#include "Controllers/SettingsController/settingscontroller.h"

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int launchMEDEA(int argc, char *argv[]){
    // If we are on windows and openend through a console pipe stdout and stderr for debugging
    #ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    #endif

    int result = 0;
    try{
        //Construct a QApplication
        QApplication a(argc, argv);

        //Initialize images
        Q_INIT_RESOURCE(images);
        Q_INIT_RESOURCE(workers);

        Theme::theme();

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

        result = a.exec();
    }catch(const std::exception& ex){
        std::cerr << "MEDEA Exception: " << ex.what() << std::endl;
        result  = 1;
    }

    //Teardown singletons
    SettingsController::teardownSettings();
    Theme::teardownTheme();
    return result;
}

int main(int argc, char *argv[])
{
    launchMEDEA(argc, argv);
}
