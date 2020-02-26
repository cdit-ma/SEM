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
    explicit ExecutionManager(ViewController* view_controller);
    
    bool HasJava();
    bool HasRe();
    bool HasRegen();

    //Functional runners
    void CheckForRe(const QString& re_configure_path);
    void CheckForRegen(const QString& re_gen_path);
    void CheckForJava();

    bool ExecuteModel(const QString& document_path, const QString& output_directory, int duration);
    void ValidateModel(const QString& model_path);
    void GenerateCodeForWorkload(const QString& document_path, ViewItem* view_item);
    
    void GenerateProject(const QString& document_path, const QString& output_directory);
    
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
    void CheckForRe_(const QString& configure_script_path);
    void CheckForRegen_(const QString& configure_script_path);
    void ValidateModel_(const QString& model_path);
    void CheckForJava_();
    void ExecuteModel_(const QString& document_path, const QString& output_directory, int duration);

    QString GetSaxonPath();

    bool GenerateProject_(const QString& document_path, const QString& output_directory);
    QString GenerateWorkload(const QString& document_path, const QString& output_directory, int id);

    void settingChanged(SETTINGS setting, const QVariant& value);

    ProcessResult RunSaxonTransform(const QString& transform_path, const QString& document, const QString& output_directory, const QStringList& arguments = QStringList());

    ViewController* view_controller_ = nullptr;
    
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
