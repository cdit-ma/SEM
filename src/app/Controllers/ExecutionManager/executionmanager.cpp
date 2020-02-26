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

// (SEM-96) NOTE - Not entirely sure if ViewController should be passed into the QObject's constructor as a parent
// This may cause issues; check/revert if that happens
ExecutionManager::ExecutionManager(ViewController *view_controller)
        : QObject(view_controller)
{
    view_controller_ = view_controller;
    transforms_path_ = QApplication::applicationDirPath() % "/Resources/re_gen/";

    connect(this, &ExecutionManager::GotJava, view_controller, &ViewController::GotJava);
    connect(this, &ExecutionManager::GotRe, view_controller, &ViewController::GotRe);
    connect(this, &ExecutionManager::GotRegen, view_controller, &ViewController::GotRegen);

    auto settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, this, &ExecutionManager::settingChanged);

    re_configured_env_ = QProcessEnvironment::systemEnvironment();

    //Run tests
    CheckForJava();

    //Force a revalidatino
    settingChanged(SETTINGS::GENERAL_RE_CONFIGURE_PATH, settings->getSetting(SETTINGS::GENERAL_RE_CONFIGURE_PATH));
    settingChanged(SETTINGS::GENERAL_REGEN_PATH, settings->getSetting(SETTINGS::GENERAL_REGEN_PATH));
}

QString ExecutionManager::GetSaxonPath(){
    return transforms_path_ % "saxon.jar";
}

bool ExecutionManager::HasJava(){
    QReadLocker lock(&lock_);
    return got_java_;
}

bool ExecutionManager::HasRegen(){
    QReadLocker lock(&lock_);
    return got_regen_;
}

bool ExecutionManager::HasRe(){
    QReadLocker lock(&lock_);
    return got_re_;
}

QString get_xml_attribute(QXmlStreamReader &xml, const QString& attribute_name)
{
    QString val;
    //Get the Attributes of the current XML entity.
    QXmlStreamAttributes attributes = xml.attributes();

    if(attributes.hasAttribute(attribute_name)){
        val = attributes.value(attribute_name).toString();
    }
    return val;
}


void ExecutionManager::ValidateModel(const QString& model_path)
{
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!validate_thread.isRunning()){
        validate_thread = QtConcurrent::run(this, &ExecutionManager::ValidateModel_, model_path);
    }
}

void ExecutionManager::ValidateModel_(const QString& model_path)
{
    auto manager =  NotificationManager::manager();

    // Clear previous validation notification items
    for (const auto& notification : manager->getNotificationsOfCategory(Notification::Category::VALIDATION)) {
        if(notification->getDescription().startsWith("model validation", Qt::CaseInsensitive)){
            continue;
        }
        manager->deleteNotification(notification->getID());
    }

    auto validation_noti = manager->AddNotification("Model validation in progress", "Icons", "shield", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::VALIDATION);
    auto results = RunSaxonTransform(transforms_path_ + "generate_validation.xsl", model_path, "");

    if (results.success) {
        int test_count = 0;
        int failed_count = 0;
        int warnings_count = 0;
        int passed_count = 0;

        QString report = results.standard_output.join("\n");
        QXmlStreamReader xml(report);

        auto start = QDateTime::currentDateTime().toMSecsSinceEpoch();
        while (!xml.atEnd()) {
            //Read each line of the xml document.
            xml.readNext();
            if(xml.isStartElement()){
                if(xml.name() == "result"){
                    auto result = get_xml_attribute(xml, "success");

                    if(result == "false"){
                        auto entity_id = get_xml_attribute(xml, "id").toInt();
                        auto is_warning = get_xml_attribute(xml, "warning") == "true";
                        auto result_text = xml.readElementText();
                        auto severity = is_warning ? Notification::Severity::WARNING : Notification::Severity::ERROR;
                        manager->AddNotification(result_text, "Icons", "circleHalo", severity, Notification::Type::MODEL, Notification::Category::VALIDATION, false, entity_id, false);
                    }
                }else if(xml.name() == "test"){
                    test_count +=  get_xml_attribute(xml, "tests").toInt();
                    passed_count +=  get_xml_attribute(xml, "passed").toInt();
                    warnings_count +=  get_xml_attribute(xml, "warnings").toInt();
                    failed_count +=  get_xml_attribute(xml, "failed").toInt();
                }
            }
        }

        //All Warnings aren't errors
        failed_count -= warnings_count;
        auto finish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        

        // Show the notification panel on validation failure
        if (passed_count < test_count) {
            emit manager->showNotificationPanel();
        }

        ///Update the original notification
        auto passed_string = "Passed: " + QString::number(passed_count) + " Tests. ";
        auto warning_string = (warnings_count > 0) ? ("Warnings: " + QString::number(warnings_count) + " Tests. ") : "";
        auto failed_string = (failed_count > 0) ? ("Failed: " + QString::number(failed_count) + " Tests. ") : "";
        validation_noti->setTitle("Validation: " + passed_string + warning_string + failed_string);
        validation_noti->setSeverity(test_count == (passed_count + warnings_count) ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    } else {
        ///Update the original notification
        validation_noti->setTitle("Model validation failed to execute");
        validation_noti->setDescription(results.standard_error.join("\n"));
        validation_noti->setSeverity(Notification::Severity::ERROR);
    }
}

void ExecutionManager::GenerateCodeForWorkload(const QString& document_path, ViewItem* item)
{
    auto id = item->getID();

    auto file_path = item->getData("label").toString() + "_" + QString::number(id) + ".cpp";
    //Get Temp Path
    auto path = FileHandler::getTempFileName("/");
    auto code = GenerateWorkload(document_path, path, id);

    if(code.length()){
        emit GotWorkloadCode(file_path, code);
    }
}

void ExecutionManager::ExecuteModel_(const QString& document_path, const QString& output_directory, int duration)
{
    if(HasRe() && HasJava() && HasRegen()){
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

        auto generate = GenerateProject_(document_path, output_directory);

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
                    notification->setTitle("Running CMake --build ..");
                    auto compile_results =  runner_->RunProcess("cmake", cmake_build_flags, build_dir, env_var);
    
                    if(compile_results.success){
                        notification->setTitle("Running model");
                        auto execute_results =  runner_->RunProcess(re_path + "re_node_manager", {"-d", document_path, "-l", "." , "-m", "tcp://127.0.0.1:7000", "-s", "tcp://127.0.0.1:7001", "-t", QString::number(duration)}, lib_dir, env_var);
    
                        if(execute_results.success){
                            notification->setTitle("Model successfully executed.");
                            failed = false;
                        }else{
                            notification->setTitle("Failed to execute model");
                            notification->setDescription(execute_results.standard_error.join("\n"));
                        }
                    }else{
                        notification->setTitle("Failed to compile model");
                        notification->setDescription(compile_results.standard_error.join("\n"));
                    }
                }else{
                    notification->setTitle("Failed to run CMake");
                    notification->setDescription(cmake_results.standard_error.join("\n"));
                }

                notification->setSeverity(failed ? Notification::Severity::ERROR : Notification::Severity::SUCCESS);
                emit ModelExecutionStateChanged(notification->getSeverity());
            }
        }
    }
}

bool ExecutionManager::GenerateProject_(const QString& document_path, const QString& output_directory)
{
    auto notification = NotificationManager::manager()->AddNotification("Generating Project C++ ...", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::FILE);
    auto results = RunSaxonTransform(transforms_path_ + "generate_project.xsl", document_path, output_directory, {});
    
    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    if(!results.success){
        notification->setTitle("Failed to generate project C++");
        notification->setDescription(results.standard_error.join("\n"));
    }else{
        notification->setTitle("Successfully generated project C++");
    }

    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);

    return results.success;
}

QString ExecutionManager::GenerateWorkload(const QString& document_path, const QString& output_directory, int id)
{
    QString arg = "id=" + QString::number(id);

    auto notification = NotificationManager::manager()->AddNotification("Generating workload C++ ...", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::FILE, false);
    auto results = RunSaxonTransform(transforms_path_ + "generate_workload.xsl", document_path, output_directory, {arg});
    
    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    if(!results.success){
        notification->setTitle("Failed to generate workload C++");
        notification->setDescription(results.standard_error.join("\n"));
    }else{
        notification->setTitle("Successfully generated workload C++");
    }
    notification->setSeverity(results.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    return results.standard_output.join("\n");
}

void ExecutionManager::settingChanged(SETTINGS setting, const QVariant& value){
    switch(setting){
        case SETTINGS::GENERAL_RE_CONFIGURE_PATH:{
            CheckForRe(value.toString());
            break;
        }
        case SETTINGS::GENERAL_REGEN_PATH:{

            QFileInfo info(value.toString() % "/");
            if(info.isAbsolute()){
                transforms_path_ = info.absolutePath() % "/";
            }else{
                transforms_path_ = QApplication::applicationDirPath() % "/" % info.path() % "/";
            }
            CheckForRegen(transforms_path_);
            break;
        }
        default:
        break;
    }
}


void ExecutionManager::CheckForRe(const QString& re_configure_path)
{
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!configure_thread.isRunning()){
        configure_thread = QtConcurrent::run(this, &ExecutionManager::CheckForRe_, re_configure_path);
    }
}

void ExecutionManager::CheckForRegen(const QString& regen_path)
{
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!regen_thread.isRunning()){
        regen_thread = QtConcurrent::run(this, &ExecutionManager::CheckForRegen_, regen_path);
    }
}


bool ExecutionManager::ExecuteModel(const QString& document_path, const QString& output_directory, int runtime_duration)
{
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!execute_model_thread.isRunning()){
        execute_model_thread = QtConcurrent::run(this, &ExecutionManager::ExecuteModel_, document_path, output_directory, runtime_duration);
        return true;
    }
    return false;
}

void ExecutionManager::GenerateProject(const QString& document_path, const QString& output_directory){
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!generate_project_thread.isRunning()){
        generate_project_thread = QtConcurrent::run(this, &ExecutionManager::GenerateProject_, document_path, output_directory);
    }
}

void ExecutionManager::CheckForJava(){
    //Gain write lock so we can set the thread object
    QWriteLocker lock(&lock_);
    if(!java_thread.isRunning()){
        java_thread = QtConcurrent::run(this, &ExecutionManager::CheckForJava_);
    }
}

//Designed to be run on a background thread
void ExecutionManager::CheckForJava_(){
    auto notification = NotificationManager::manager()->AddNotification("Checking for Java", "Icons", "java", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);
    ProcessRunner runner;
    auto result = runner.RunProcess("java", {"-version"});
    
    notification->setTitle(result.success ? "Found Java" : "Cannot find Java");
    notification->setSeverity(result.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    notification->setDescription(result.standard_error.join("\n"));
    {
        QWriteLocker lock(&lock_);
        got_java_ = result.success;
    }
    emit GotJava(result.success);
}


void ExecutionManager::CheckForRegen_(const QString& regen_path){
    auto notification = NotificationManager::manager()->AddNotification("Checking for Regen", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);
    
    QFileInfo file(regen_path + "/saxon.jar");

    bool saxon_exists = file.exists() && file.size();

    notification->setSeverity(saxon_exists ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    notification->setTitle(saxon_exists ? "Found re_gen" : "Cannot find re_gen");
    notification->setDescription(notification->getTitle() + " in: '" + regen_path + "'");

    {
        QWriteLocker lock(&lock_);
        got_regen_ = saxon_exists;
    }
    emit GotRegen(saxon_exists);
}

//Designed to be run on a background thread
void ExecutionManager::CheckForRe_(const QString& re_configure_path){
    auto notification = NotificationManager::manager()->AddNotification("Checking for Re", "Icons", "servers", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::NONE);
    
    bool success = false;
    QString status;
    QProcessEnvironment re_env;
    if(FileHandler::isFileReadable(re_configure_path)){
        ProcessRunner runner;
        
        re_env = runner.RunEnvVarScript(re_configure_path);
        
        if(re_env.contains("RE_PATH")){
            success = true;
            status = "Found RE: '" + re_env.value("RE_PATH") + "'";
        }else{
            status = "RE configure script doesn't define RE_PATH";
        }
    }else if(re_configure_path == ""){
        status = "RE configure script hasn't been set";
    }else{
        status = "RE configure script doesn't point to a readable file '" + re_configure_path + "'";
    }

    notification->setSeverity(success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    notification->setTitle(success ? "Found Re" : "Cannot find Re");
    notification->setDescription(status);

    {
        QWriteLocker lock(&lock_);
        got_re_ = success;
        if(got_re_){
            re_configured_env_ = re_env;
        }
    }
    emit GotRe(got_re_);
}

ProcessResult ExecutionManager::RunSaxonTransform(const QString& transform_path, const QString& document, const QString& output_directory, const QStringList& arguments)
{
    ProcessRunner runner;
    auto runner_ = &runner;
    
    QString program = "java";

    QStringList args;
    args << "-jar";
    args << GetSaxonPath();
    args << "-xsl:" + transform_path;
    args << "-s:" + document;
    args << arguments;

    FileHandler::ensureDirectory(output_directory);
    return runner_->RunProcess(program, args, output_directory);
}
