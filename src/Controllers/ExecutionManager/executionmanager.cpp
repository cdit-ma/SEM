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

    connect(this, &ExecutionManager::GotJava, view_controller, &ViewController::GotJava);
    connect(this, &ExecutionManager::GotRe, view_controller, &ViewController::GotRe);


    auto settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, this, &ExecutionManager::settingChanged);

    re_configured_env_ = QProcessEnvironment::systemEnvironment();

    //Run tests
    CheckForJava();

    //Force a revalidatino
    settingChanged(SETTINGS::GENERAL_RE_CONFIGURE_PATH, settings->getSetting(SETTINGS::GENERAL_RE_CONFIGURE_PATH));
}

bool ExecutionManager::HasJava(){
    QReadLocker lock(&lock_);
    return got_java_;
}

bool ExecutionManager::HasRe(){
    QReadLocker lock(&lock_);
    return got_re_;
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
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!validate_thread.isRunning()){
        validate_thread = QtConcurrent::run(QThreadPool::globalInstance(), this, &ExecutionManager::ValidateModel_, model_path);
    }
}

void ExecutionManager::ValidateModel_(QString model_path)
{
    auto manager =  NotificationManager::manager();

    // Clear previous validation notification items
    for (auto notification : manager->getNotificationsOfCategory(Notification::Category::VALIDATION)) {
        if(notification->getDescription().startsWith("model validation", Qt::CaseInsensitive)){
            continue;
        }
        manager->deleteNotification(notification->getID());
    }

    auto validation_noti = manager->AddNotification("Model validation in progress", "Icons", "shield", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::VALIDATION);
    auto results = RunSaxonTransform(transforms_path_ + "g2validate.xsl", model_path, "");

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
                        
                        manager->AddNotification(result_text, "Icons", "circleHalo", severity, Notification::Type::MODEL, Notification::Category::VALIDATION, false, entity_id, true);
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

void ExecutionManager::ExecuteModel_(QString document_path, QString output_directory, int duration){
    if(HasRe() && HasJava()){
        ProcessRunner runner;
        auto runner_ = &runner;

        
        connect(this, &ExecutionManager::CancelModelExecution, runner_, &ProcessRunner::Cancel, Qt::QueuedConnection);
        connect(runner_, &ProcessRunner::GotProcessStdOutLine, this, &ExecutionManager::GotProcessStdOutLine, Qt::QueuedConnection);
        connect(runner_, &ProcessRunner::GotProcessStdErrLine, this, &ExecutionManager::GotProcessStdErrLine, Qt::QueuedConnection);

        QProcessEnvironment env_var;
        {
            QReadLocker lock(&lock_);
            env_var = re_configured_env_;
        }

        auto re_path = env_var.value("RE_PATH") + "/bin/";
        qCritical() << "RE_PATH: '" << re_path << "'";

        auto generate = GenerateWorkspace_(document_path, output_directory);

        auto notification = NotificationManager::manager()->AddNotification("Running CMake...", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::FILE);
        emit ModelExecutionStateChanged(notification->getSeverity());
        if(generate){
            auto build_dir = output_directory + "/build/";
            auto lib_dir = output_directory + "/lib/";
            auto cmake_generator = SettingsController::settings()->getSetting(SETTINGS::GENERAL_CMAKE_GENERATOR).toString();

            QStringList cmake_flags = {"-G", cmake_generator, ".."};

            #ifdef WIN32
                cmake_flags += "-DCMAKE_BUILD_TYPE=Release";
            #endif
            QStringList cmake_build_flags = {"--build", ".", "--config", "Release"};

            bool failed = true;
            
            //Clean and rerun
            if(FileHandler::removeDirectory(build_dir) && FileHandler::ensureDirectory(build_dir)){
                auto cmake_results = runner_->RunProcess("cmake", cmake_flags, build_dir, env_var);
    
                if(cmake_results.success){
                    notification->setDescription("Running CMake --build ..");
                    auto compile_results =  runner_->RunProcess("cmake", cmake_build_flags, build_dir, env_var);
    
                    if(compile_results.success){
                        notification->setDescription("Running model");
                        auto execute_results =  runner_->RunProcess(re_path + "re_node_manager", {"-d", document_path, "-l", "." , "-m", "tcp://127.0.0.1:7000", "-s", "tcp://127.0.0.1:7001", "-t", QString::number(duration)}, lib_dir, env_var);
    
                        if(execute_results.success){
                            notification->setDescription("Model successfully executed.");
                            failed = false;
                        }else{
                            notification->setDescription("Failed to execute model");
                        }
                    }else{
                        notification->setDescription("Failed to compile model");
                    }
                }else{
                    notification->setDescription("Failed to run CMake");
                }

                notification->setSeverity(failed ? Notification::Severity::ERROR : Notification::Severity::SUCCESS);
                emit ModelExecutionStateChanged(notification->getSeverity());
            }
        }
    }
}

bool ExecutionManager::GenerateWorkspace_(QString document_path, QString output_directory)
{
    auto notification = NotificationManager::manager()->AddNotification("Generating model workspace C++ ...", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::FILE);

    auto components = GenerateComponents(document_path, output_directory, {}, false);
    auto datatypes = GenerateDatatypes(document_path, output_directory, false);

    
    if(components && datatypes){
        notification->setDescription("Successfully generated model workspace C++ in '" + output_directory + "'");
    }else{
        notification->setDescription("Generated model workspace C++ failed!");
    }
    notification->setSeverity(components && datatypes ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    return components && datatypes;
}

bool ExecutionManager::GenerateComponents(QString document_path, QString output_directory, QStringList component_names, bool toast_notify)
{
    QStringList args;
    if(component_names.size() > 0){
        args << "components=" + component_names.join(",");
        args << "preview=true";
    }

    auto notification = NotificationManager::manager()->AddNotification("Generating component C++ ...", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::FILE, toast_notify);
    auto results = RunSaxonTransform(transforms_path_ + "g2components.xsl", document_path, output_directory, args);
    
    if(!results.success){
        notification->setDescription("Generating component C++ failed! '" + results.standard_error.join("") + "'");
    }else{
        notification->setDescription("Successfully generated component C++");
    }

    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);

    return results.success;
}

bool ExecutionManager::GenerateDatatypes(QString document_path, QString output_directory, bool toast_notify)
{
    // Construct a notification item with a loading gif as its icon
    auto notification = NotificationManager::manager()->AddNotification("Generating datatype C++ ...", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::FILE, toast_notify);
    auto results = RunSaxonTransform(transforms_path_ + "g2datatypes.xsl", document_path, output_directory, GetMiddlewareArgs());
    
    if(!results.success){
        notification->setDescription("Generating datatype C++ failed! '" + results.standard_error.join("") + "'");
    }else{
        notification->setDescription("Successfully generated datatype C++");
    }
    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);

    

    return results.success;
}

QStringList ExecutionManager::GetMiddlewareArgs()
{

    QStringList args;
    args << "middlewares=zmq,proto,rti,qpid,ospl";
    return args;
}

void ExecutionManager::settingChanged(SETTINGS setting, QVariant value){
    switch(setting){
        case SETTINGS::GENERAL_RE_CONFIGURE_PATH:{
            CheckForRe(value.toString());
            break;
        }
        default:
        break;
    }
}

void ExecutionManager::CheckForRe(QString re_configure_path)
{
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!configure_thread.isRunning()){
        configure_thread = QtConcurrent::run(QThreadPool::globalInstance(), this, &ExecutionManager::CheckForRe_, re_configure_path);
    }
}

bool ExecutionManager::ExecuteModel(QString document_path, QString output_directory, int runtime_duration)
{
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!execute_model_thread.isRunning()){
        execute_model_thread = QtConcurrent::run(QThreadPool::globalInstance(), this, &ExecutionManager::ExecuteModel_, document_path, output_directory, runtime_duration);
        return true;
    }
    return false;
}

void ExecutionManager::GenerateWorkspace(QString document_path, QString output_directory){
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!generate_workspace_thread.isRunning()){
        generate_workspace_thread = QtConcurrent::run(QThreadPool::globalInstance(), this, &ExecutionManager::GenerateWorkspace_, document_path, output_directory);
    }
}

void ExecutionManager::CheckForJava(){
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!java_thread.isRunning()){
        java_thread = QtConcurrent::run(QThreadPool::globalInstance(), this, &ExecutionManager::CheckForJava_);
    }
}

//Designed to be run on a background thread
void ExecutionManager::CheckForJava_(){
    ProcessRunner runner;
    auto runner_ = &runner;
    auto result = runner_->RunProcess("java", {"-version"});
    {
        QWriteLocker lock(&lock_);
        got_java_ = result.success;
    }
    emit GotJava(result.success, result.standard_error.join(""));
}

//Designed to be run on a background thread
void ExecutionManager::CheckForRe_(QString re_configure_path){
    ProcessRunner runner;
    auto runner_ = &runner;
    //Run 
    bool got_re = false;
    QString status;

    auto new_env = runner_->RunEnvVarScript(re_configure_path);
    
    //Check if we have RE_PATH
    if(new_env.contains("RE_PATH")){
        got_re = true;
        status = "Found RE: '" + new_env.value("RE_PATH") + "'";
    }else{
        if(FileHandler::isFileReadable(re_configure_path)){
            status = "RE configure script doesn't appear to define RE_PATH '" + re_configure_path + "'";
        }else{
            status = "RE configure script doesn't appear to point to a readable file '" + re_configure_path + "'";
        }
    }

    {
        QWriteLocker lock(&lock_);
        got_re_ = got_re;
        if(got_re_){
            re_configured_env_ = new_env;
        }
    }
    emit GotRe(got_re_, status);
}

ProcessResult ExecutionManager::RunSaxonTransform(QString transform_path, QString document, QString output_directory, QStringList arguments)
{
    ProcessRunner runner;
    auto runner_ = &runner;
    
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
