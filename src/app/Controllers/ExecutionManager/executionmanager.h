#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include "../../Widgets/Monitors/monitor.h"
#include "../../Utils/processrunner.h"
#include "../SettingsController/settingscontroller.h"

#include <QProcessEnvironment>
#include <QObject>
#include <QMutex>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QThreadPool>
#include <QReadWriteLock>

class ViewItem;
class ViewController;
class ExecutionManager: public QObject
{
    Q_OBJECT
public:
    ExecutionManager(ViewController* view_controller);
    bool HasJava();
    bool HasRe();

    //Functional runners
    void CheckForRe(QString re_configure_path);
    void CheckForJava();

    


    void RunConfigure(QString configure_script_path);
    bool ExecuteModel(QString document_path, QString output_directory, int duration);
    void ValidateModel(QString model_path);
    void GenerateCodeForWorkload(QString document_path, ViewItem* view_item);
    
    void GenerateWorkspace(QString document_path, QString output_directory);
    QString get_env_var(QString key);
signals:
    void GotProcessStdOutLine(QString line);
    void GotProcessStdErrLine(QString line);
    void ModelExecutionStateChanged(Notification::Severity state);

    void CancelModelExecution();
    void GotWorkloadCode(QString file_name, QString cpp_code);

    void GotJava(bool ready);
    void GotRe(bool ready);
private:
    
    void CheckForRe_(QString configure_script_path);
    void ValidateModel_(QString model_path);
    void CheckForJava_();
    void ExecuteModel_(QString document_path, QString output_directory, int duration);

    bool GenerateWorkspace_(QString document_path, QString output_directory);
    bool GenerateComponents(QString document_path, QString output_directory, QStringList component_names=QStringList(), bool toast_notify = true);
    bool GenerateDatatypes(QString document_path, QString output_directory, bool toast_notify = true);

    QString GenerateWorkload(QString document_path, QString output_directory, int id);



    void settingChanged(SETTINGS setting, QVariant value);
    QStringList GetMiddlewareArgs();

    


    ProcessResult RunSaxonTransform(QString transform_path, QString document, QString output_directory, QStringList arguments=QStringList());

    //ProcessRunner* runner_ = 0;
    ViewController* view_controller_ = 0;;
    
    QProcessEnvironment re_configured_env_;
    QString saxon_jar_path_;
    QString transforms_path_;

    QFuture<void> configure_thread;
    QFuture<void> java_thread;
    QFuture<void> validate_thread;
    QFuture<void> execute_model_thread;
    QFuture<void> generate_workspace_thread;
    QFuture<void> generate_code_thread;

    QReadWriteLock lock_;
    
    bool got_java_ = false;
    bool got_re_ = false;
};


#endif // EXECUTIONMANAGER_H
