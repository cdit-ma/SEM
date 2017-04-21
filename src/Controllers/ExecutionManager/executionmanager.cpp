#include "executionmanager.h"
#include <QApplication>
#include <QStringBuilder>
#include <QDebug>
#include "../../Utils/filehandler.h"
#include "../NotificationManager/notificationmanager.h"
#include "../ViewController/viewcontroller.h"

ExecutionManager::ExecutionManager(ViewController *view_controller)
{
    view_controller_ = view_controller;
    transforms_path_ = QApplication::applicationDirPath() % "/resources/re_gen/";
    saxon_jar_path_ = transforms_path_ % "saxon.jar";
    runner_ = new ProcessRunner(this);
    got_java_ = false;

    connect(this, &ExecutionManager::GotJava, view_controller, &ViewController::jenkinsManager_GotJava);

    //run check for java
    GotJava_();
}


void ExecutionManager::ValidateModel(QString model_path, QString output_file_path)
{

}

void ExecutionManager::GenerateCodeForComponent(QString document_path, QString component_name)
{
    //Get Temp Path
    QString path = FileHandler::getTempFileName("/");

    component_name = component_name.toLower();
    bool results = GenerateComponents(document_path, path, QStringList(component_name));
    if(results){
        QStringList exts;
        exts << "impl.cpp" << "impl.h";
        foreach(auto ext, exts){
            QString file_name = component_name + ext;
            QString file_path = path + "/components/" + component_name + "/" + file_name;
            QString file_data = FileHandler::readTextFile(file_path);
            emit GotCodeForComponent(file_name, file_data);
        }
    }
    //Destroy temp directory
    FileHandler::removeDirectory(path);
}

void ExecutionManager::GenerateWorkspace(QString document_path, QString output_directory)
{
    auto components = GenerateComponents(document_path, output_directory);
    auto datatypes = GenerateDatatypes(document_path, output_directory);

    if(components && datatypes){
        NotificationManager::manager()->displayNotification("Generated Model Workspace in '" + output_directory + "'", "Icons", "bracketsAngled", -1, NS_INFO, NT_MODEL, NC_VALIDATION);
    }
}

bool ExecutionManager::GenerateComponents(QString document_path, QString output_directory, QStringList component_names)
{
    QStringList args;
    if(component_names.size() > 0){
        args << "components=" + component_names.join(",");
        args << "preview=true";
    }

    emit NotificationManager::manager()->backgroundProcess(true, BP_VALIDATION);
    auto results = RunSaxonTransform(transforms_path_ + "g2components.xsl", document_path, output_directory, args);
    emit NotificationManager::manager()->backgroundProcess(false, BP_VALIDATION);

    if(!results.success){
        NotificationManager::manager()->displayNotification("XSL Generation of Components Failed: '" + results.standard_error.join("") + "'", "Icons", "bracketsAngled", -1, NS_ERROR, NT_MODEL, NC_NOCATEGORY);
    }
    return results.success;
}

bool ExecutionManager::GenerateDatatypes(QString document_path, QString output_directory)
{
    emit NotificationManager::manager()->backgroundProcess(true, BP_VALIDATION);
    auto results = RunSaxonTransform(transforms_path_ + "g2datatypes.xsl", document_path, output_directory, GetMiddlewareArgs());
    emit NotificationManager::manager()->backgroundProcess(false, BP_VALIDATION);

    if(!results.success){
        NotificationManager::manager()->displayNotification("XSL Generation of Datatypes Failed: '" + results.standard_error.join("") + "'", "Icons", "bracketsAngled", -1, NS_ERROR, NT_MODEL, NC_NOCATEGORY);
    }
    return results.success;
}

QStringList ExecutionManager::GetMiddlewareArgs()
{

    QStringList args;
    args << "middlewares=zmq,proto,rti,qpid,ospl";
    return args;
}

bool ExecutionManager::GotJava_()
{
    if(!got_java_){
        QString program = "java";
        QStringList args;
        args << "-version";

        auto result = runner_->RunProcess(program, args);

        emit GotJava(result.success, result.standard_error.join(""));
        got_java_ = result.success;
    }
}

ProcessResult ExecutionManager::RunSaxonTransform(QString transform_path, QString document, QString output_directory, QStringList arguments)
{
    QString program = "java";

    QStringList args;
    args << "-jar";
    args << saxon_jar_path_;
    args << "-xsl:" + transform_path;
    args << "-s:" + document;
    args << arguments;

    FileHandler::ensureDirectory(output_directory);
    return runner_->RunProcess(program, args, output_directory);
}
