#include "executionmanager.h"
#include <QApplication>
#include <QStringBuilder>
#include <QDebug>
#include <QXmlStreamReader>
#include <QMutexLocker>

#include "../../Utils/filehandler.h"
#include "../NotificationManager/notificationmanager.h"
#include "../NotificationManager/notificationobject.h"
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
    {
        QMutexLocker locker(&this->mutex_);
        if(running_validation){
            return;
        }
        running_validation = true;
    }
    auto manager =  NotificationManager::manager();

    // Clear previous validation notification items
    for (auto notification : manager->getNotificationsOfCategory(Notification::Category::VALIDATION)) {
        if(notification->getDescription().startsWith("model validation", Qt::CaseInsensitive)){
            continue;
        }
        manager->deleteNotification(notification->getID());
    }

    auto validation_noti = manager->AddNotification("Model validation in progress", "Icons", "shield", Notification::Severity::INFO, Notification::Type::MODEL, Notification::Category::VALIDATION, true);
    auto results = RunSaxonTransform(transforms_path_ + "g2validate.xsl", model_path, "");
    validation_noti->setInProgressState(false);

    if (results.success) {
        int test_count = 0;
        int success_count = 0;

        QString report = results.standard_output.join("");
        QXmlStreamReader xml(report);

        auto start = QDateTime::currentDateTime().toMSecsSinceEpoch();
        while (!xml.atEnd()) {
            //Read each line of the xml document.
            xml.readNext();
            if(xml.isStartElement()){
                if(xml.name() == "result"){
                    test_count ++;
                    auto result = get_xml_attribute(xml, "success");

                    if(result == "false"){
                        auto entity_id = get_xml_attribute(xml, "id").toInt();
                        auto is_warning = get_xml_attribute(xml, "warning") == "true";
                        auto result_text = xml.readElementText();
                        auto severity = is_warning ? Notification::Severity::WARNING : Notification::Severity::ERROR;
                        
                        manager->AddNotification(result_text, "Icons", "circleHalo", severity, Notification::Type::MODEL, Notification::Category::VALIDATION, false, false, entity_id, true);
                    }else if(result == "true"){
                        success_count ++;
                    }
                }
            }
        }
        auto finish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qCritical() << "Panel rendering notifications: " <<  finish - start << "MS";
        

        // Show the notification panel on validation failure
        if (success_count < test_count) {
            emit manager->showNotificationPanel();
        }

        ///Update the original notification
        validation_noti->setDescription("Model validation - [" + QString::number(success_count) + "/" + QString::number(test_count) + "] tests passed");
        validation_noti->setSeverity(test_count == success_count ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    } else {
        ///Update the original notification
        validation_noti->setDescription("Model validation failed to execute: '" + results.standard_error.join("") + "'");
        validation_noti->setSeverity(Notification::Severity::ERROR);
    }

    {
        QMutexLocker locker(&this->mutex_);
        running_validation = false;
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
    auto notification = NotificationManager::manager()->AddNotification("Generating model workspace C++ ...", "Icons", "bracketsAngled", Notification::Severity::INFO, Notification::Type::MODEL, Notification::Category::FILE, true);

    auto components = GenerateComponents(document_path, output_directory, {}, false);
    auto datatypes = GenerateDatatypes(document_path, output_directory, false);
    notification->setInProgressState(false);

    
    notification->setSeverity(components && datatypes ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    if(components && datatypes){
        notification->setDescription("Successfully generated model workspace C++ in '" + output_directory + "'");
    }else{
        notification->setDescription("Generated model workspace C++ failed!");
    }
}

bool ExecutionManager::GenerateComponents(QString document_path, QString output_directory, QStringList component_names, bool toast_notify)
{
    QStringList args;
    if(component_names.size() > 0){
        args << "components=" + component_names.join(",");
        args << "preview=true";
    }

    auto notification = NotificationManager::manager()->AddNotification("Generating component C++ ...", "Icons", "bracketsAngled", Notification::Severity::INFO, Notification::Type::MODEL, Notification::Category::FILE, true, toast_notify);
    auto results = RunSaxonTransform(transforms_path_ + "g2components.xsl", document_path, output_directory, args);
    notification->setInProgressState(false);
    
    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    if(!results.success){
        notification->setDescription("Generating component C++ failed! '" + results.standard_error.join("") + "'");
    }else{
        notification->setDescription("Successfully generated component C++");
    }

    return results.success;
}

bool ExecutionManager::GenerateDatatypes(QString document_path, QString output_directory, bool toast_notify)
{
    // Construct a notification item with a loading gif as its icon
    auto notification = NotificationManager::manager()->AddNotification("Generating datatype C++ ...", "Icons", "bracketsAngled", Notification::Severity::INFO, Notification::Type::MODEL, Notification::Category::FILE, true, toast_notify);
    auto results = RunSaxonTransform(transforms_path_ + "g2datatypes.xsl", document_path, output_directory, GetMiddlewareArgs());
    notification->setInProgressState(false);
    
    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    if(!results.success){
        notification->setDescription("Generating datatype C++ failed! '" + results.standard_error.join("") + "'");
    }else{
        notification->setDescription("Successfully generated datatype C++");
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
