#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>

#include "../modelcontroller/modelcontroller.h"
#include "../modelcontroller/utils.h"
#include "../modelcontroller/version.h"

int main(int argc, char** argv){
    auto start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("medea_cli");
    QCoreApplication::setApplicationVersion("v" + APP_VERSION());

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption open_options({"o", "open"}, "Open a graphml project.", "The graphml file path");
    QCommandLineOption import_options({"i", "import"}, "Import graphml project(s).", "The graphml file path");
    QCommandLineOption export_options({"e", "export"}, "Export project as a graphml file.", "The graphml file path");
    QCommandLineOption function_export_options({"f", "functional"}, "Functionally export, stripping out visual information.");

    parser.addOption(open_options);
    parser.addOption(import_options);
    parser.addOption(export_options);
    parser.addOption(function_export_options);
    parser.process(app);

    int error_count = 0;

    //Only one project
    auto open_project = parser.value(open_options);
    auto import_projects = parser.values(import_options);
    auto export_project = parser.value(export_options);
    auto functional_export = parser.isSet(function_export_options);

    bool got_values = parser.isSet(open_options) || parser.isSet(import_options);

    if(!got_values){
        //parser.showVersion();
        parser.showHelp(1);
    }
    
    QScopedPointer<ModelController> controller(new ModelController(QCoreApplication::applicationDirPath()));

    //Setup the controller, passing in the project to open if we have any
    auto setup_success = controller->SetupController(open_project);
    if(open_project.length()){
        qInfo() << "Opening Project: " << open_project << (setup_success ? "[SUCCESS]" : "[FAIL]");
    }else{
        qInfo() << "Setting up Controller: " << (setup_success ? "[SUCCESS]" : "[FAIL]");
    }
    auto after_open = QDateTime::currentDateTime().toMSecsSinceEpoch();

    qInfo() << "Took: " << (after_open - start) << " MS";


    error_count += setup_success;


    if(import_projects.size()){
        auto import_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
        for(auto file_path : import_projects){
            auto success = controller->importProjects({Utils::readTextFile(file_path)});
            qInfo() << "Importing Project:" << file_path << (success ? "[SUCCESS]" : "[FAIL]");
            error_count += success;
        }
        auto import_end = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qInfo() << "Importing: " << import_projects.size() << " Projects took: " << (import_end - import_start) << " MS";
    }
    

    //Export Model as graphml
    if(export_project.length()){
        auto export_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
        auto data = controller->getProjectAsGraphML(functional_export);
        auto success = Utils::writeTextFile(export_project, data);
        qInfo() << "Exporting Project:" << export_project << (success ? "[SUCCESS]" : "[FAIL]");
        error_count += success;
        auto export_end = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qInfo() << "Exporting Took: " << (export_end - export_start) << " MS";
    }


    auto destruct_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    controller.reset();
    auto destruct_end = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qInfo() << "Destruct took: " << (destruct_end - destruct_start) << " MS";
    qInfo() << "Total Time: " << (destruct_end - start) << " MS";
    return error_count;
}