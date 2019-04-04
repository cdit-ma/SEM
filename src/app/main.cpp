#include <QApplication>
#include <QString>
#include <QCommandLineParser>

#include "Controllers/ViewController/viewcontroller.h"
#include "Controllers/WindowManager/windowmanager.h"
#include "Widgets/Windows/mainwindow.h"
#include "Controllers/SettingsController/settingscontroller.h"
#include "../modelcontroller/version.h"

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int launchMEDEA(int argc, char *argv[]){
    // If we are on windows and openend through a console pipe stdout and stderr for debugging
    #ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* file;
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONOUT$", "w", stderr);
    }
    #endif

    QScopedPointer<ViewController> view_controller;

    int result = 0;
    try{
        //Construct a QApplication
        QApplication a(argc, argv);

        //Fixes MacOS QIcon resolution.
        a.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

        //Initialize images
        Q_INIT_RESOURCE(images);
        Q_INIT_RESOURCE(workers);

        //Setup QApplication
        a.setApplicationName("MEDEA");
        a.setApplicationVersion(APP_VERSION());
        a.setOrganizationName("CDIT-MA");
        a.setOrganizationDomain("https://github.com/cdit-ma/");

        //Handle Command line options
        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addPositionalArgument("file", "Open a graphml project");
        parser.process(a);

        auto project_paths = parser.positionalArguments();


        //Initialize images
        Q_INIT_RESOURCE(images);
        Q_INIT_RESOURCE(workers);


        //Initialize important singletons
        Theme::theme();
        SettingsController::settings();
        
        view_controller.reset(new ViewController());
        
        auto window = WindowManager::manager()->constructMainWindow(view_controller.data());
        if (project_paths.size() == 1) {
            view_controller->OpenExistingProject(project_paths.first());
        }
        a.setActiveWindow(window);

        result = a.exec();
    }catch(const std::exception& ex){
        std::cerr << "MEDEA Exception: " << ex.what() << std::endl;
        result  = 1;
    }
    return result;
}

int main(int argc, char *argv[])
{
    launchMEDEA(argc, argv);
}
