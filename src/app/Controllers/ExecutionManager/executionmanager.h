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
    bool HasRegen();

    //Functional runners
    void CheckForRe(QString re_configure_path);
    void CheckForRegen(QString re_gen_path);
    void CheckForJava();

    


    void RunConfigure(QString configure_script_path);
    bool ExecuteModel(QString document_path, QString output_directory, int duration);
    void ValidateModel(QString model_path);
    void GenerateCodeForWorkload(QString document_path, ViewItem* view_item);
    
    void GenerateProject(QString document_path, QString output_directory);
    QString get_env_var(QString key);
signals:
    void GotProcessStdOutLine(QString line);
    void GotProcessStdErrLine(QString line);
    void ModelExecutionStateChanged(Notification::Severity state);

    void CancelModelExecution();
    void GotWorkloadCode(QString file_name, QString cpp_code);

    void GotJava(bool ready);
    void GotRe(bool ready);
    void GotRegen(bool ready);
private:
    void CheckForRe_(QString configure_script_path);
    void CheckForRegen_(QString configure_script_path);
    void ValidateModel_(QString model_path);
    void CheckForJava_();
    void ExecuteModel_(QString document_path, QString output_directory, int duration);

    QString GetSaxonPath();

    bool GenerateProject_(QString document_path, QString output_directory);
    QString GenerateWorkload(QString document_path, QString output_directory, int id);


    void settingChanged(SETTINGS setting, QVariant value);

    


    ProcessResult RunSaxonTransform(QString transform_path, QString document, QString output_directory, QStringList arguments=QStringList());

    //ProcessRunner* runner_ = 0;
    ViewController* view_controller_ = 0;;
    
    QProcessEnvironment re_configured_env_;
    QString transforms_path_;

    QFuture<void> configure_thread;
    QFuture<void> java_thread;
    QFuture<void> validate_thread;
    QFuture<void> execute_model_thread;
    QFuture<bool> generate_project_thread;
    QFuture<void> generate_code_thread;
    QFuture<void> regen_thread;

    QReadWriteLock lock_;
    
    bool got_java_ = false;
    bool got_re_ = false;
    bool got_regen_ = false;
};


#endif // EXECUTIONMANAGER_H
