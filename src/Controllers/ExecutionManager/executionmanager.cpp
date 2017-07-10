#include "executionmanager.h"
#include <QApplication>
#include <QStringBuilder>
#include <QDebug>
#include <QXmlStreamReader>

#include "../../Utils/filehandler.h"
#include "../NotificationManager/notificationmanager.h"
#include "../ViewController/viewcontroller.h"

ExecutionManager::ExecutionManager(ViewController *view_controller)
{
    view_controller_ = view_controller;
    transforms_path_ = QApplication::applicationDirPath() % "/Resources/re_gen/";
    saxon_jar_path_ = transforms_path_ % "saxon.jar";
    runner_ = new ProcessRunner(this);
    got_java_ = false;

    connect(this, &ExecutionManager::GotJava, view_controller, &ViewController::jenkinsManager_GotJava);

    //connect(this, &ExecutionManager::GotValidationReport, NotificationManager::manager(), &NotificationManager::modelValidated);
    //run check for java
    GotJava_();
}


QString get_xml_attribute(QXmlStreamReader &xml, QString attribute_name)
{
    QString val;
    //Get the Attributes of the current XML entity.
    QXmlStreamAttributes attributes = xml.attributes();

    if(attributes.hasAttribute(attribute_name)){
        val = attributes.value(attribute_name).toString();
    }
    return val;
}

void ExecutionManager::ValidateModel(QString model_path)
{
    // Clear previous validation notification items
    foreach (int ID, NotificationManager::manager()->getNotificationsOfCategory(NOTIFICATION_CATEGORY::VALIDATION)) {
        NotificationManager::manager()->deleteNotification(ID);
    }

    // Construct a notification item with a loading gif as its icon
    int nID = NotificationManager::displayLoadingNotification("Model validation in progress...", "Icons", "shield", -1, NOTIFICATION_SEVERITY::INFO, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::VALIDATION);
    auto results = RunSaxonTransform(transforms_path_ + "g2validate.xsl", model_path, "");
    NotificationManager::manager()->deleteNotification(nID);

    if (results.success) {

        int count = 0;
        int success_count = 0;

        QString report = results.standard_output.join("");
        QXmlStreamReader xml(report);
        QString current_test;

        while (!xml.atEnd()) {
            //Read each line of the xml document.
            xml.readNext();
            auto tag_name = xml.name();

            if(xml.isStartElement()){
                if(tag_name == "test"){
                    current_test = get_xml_attribute(xml, "name");

                }else if(tag_name == "result"){
                    auto result = get_xml_attribute(xml, "success");
                    count ++;

                    if(result == "false"){
                        auto id = get_xml_attribute(xml, "id");
                        auto warning = get_xml_attribute(xml, "warning") == "true";
                        auto error_code = xml.readElementText();
                        NotificationManager::displayNotification(error_code, "Icons", "circleHalo", id.toInt(), warning? NOTIFICATION_SEVERITY::WARNING : NOTIFICATION_SEVERITY::ERROR, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::VALIDATION);
                    }else if(result == "true"){
                        success_count ++;
                    }
                }
            }
        }

        // Show the notification panel on validation failure
        if (success_count < count) {
            emit NotificationManager::manager()->showNotificationPanel();
        }
        NotificationManager::displayNotification("Model Validation - [" + QString::number(success_count) + "/" + QString::number(count) + "] tests passed", "Icons", "shield", -1, success_count == count ? NOTIFICATION_SEVERITY::INFO : NOTIFICATION_SEVERITY::ERROR, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::VALIDATION);

    } else {
        NotificationManager::displayNotification("XSL Validation failed: '" + results.standard_error.join("") + "'", "Icons", "shield", -1, NOTIFICATION_SEVERITY::ERROR, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::VALIDATION);
    }
}

void ExecutionManager::GenerateCodeForComponent(QString document_path, QString component_name)
{
    //Get Temp Path
    QString path = FileHandler::getTempFileName("/");
    //
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
        NotificationManager::manager()->displayNotification("Generated Model Workspace in '" + output_directory + "'", "Icons", "bracketsAngled", -1, NOTIFICATION_SEVERITY::INFO, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::VALIDATION);
    }
}

bool ExecutionManager::GenerateComponents(QString document_path, QString output_directory, QStringList component_names)
{
    QStringList args;
    if(component_names.size() > 0){
        args << "components=" + component_names.join(",");
        args << "preview=true";
    }

    //emit NotificationManager::manager()->backgroundProcess(true, BACKGROUND_PROCESS::VALIDATION);
    //auto results = RunSaxonTransform(transforms_path_ + "g2components.xsl", document_path, output_directory, args);
    //emit NotificationManager::manager()->backgroundProcess(false, BACKGROUND_PROCESS::VALIDATION);

    // Construct a notification item with a loading gif as its icon
    int nID = NotificationManager::displayLoadingNotification("Model validation in progress...", "Icons", "shield", -1, NOTIFICATION_SEVERITY::INFO, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::VALIDATION);
    auto results = RunSaxonTransform(transforms_path_ + "g2components.xsl", document_path, output_directory, args);
    NotificationManager::manager()->deleteNotification(nID);

    if(!results.success){
        NotificationManager::displayNotification("XSL Generation of Components Failed: '" + results.standard_error.join("") + "'", "Icons", "bracketsAngled", -1, NOTIFICATION_SEVERITY::ERROR, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::NOCATEGORY);
    }
    return results.success;
}

bool ExecutionManager::GenerateDatatypes(QString document_path, QString output_directory)
{
    //emit NotificationManager::manager()->backgroundProcess(true, BACKGROUND_PROCESS::VALIDATION);
    //auto results = RunSaxonTransform(transforms_path_ + "g2datatypes.xsl", document_path, output_directory, GetMiddlewareArgs());
    //emit NotificationManager::manager()->backgroundProcess(false, BACKGROUND_PROCESS::VALIDATION);

    // Construct a notification item with a loading gif as its icon
    int nID = NotificationManager::displayLoadingNotification("Model validation in progress...", "Icons", "shield", -1, NOTIFICATION_SEVERITY::INFO, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::VALIDATION);
    auto results = RunSaxonTransform(transforms_path_ + "g2datatypes.xsl", document_path, output_directory, GetMiddlewareArgs());
    NotificationManager::manager()->deleteNotification(nID);

    if(!results.success){
        NotificationManager::manager()->displayNotification("XSL Generation of Datatypes Failed: '" + results.standard_error.join("") + "'", "Icons", "bracketsAngled", -1, NOTIFICATION_SEVERITY::ERROR, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::NOCATEGORY);
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
    return got_java_;
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
