#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include "../../Utils/processrunner.h"
#include <QObject>
#include <QMutex>
class ViewController;
class ExecutionManager: public QObject
{
    Q_OBJECT
public:
    ExecutionManager(ViewController* view_controller);

    void ValidateModel(QString model_path);
    void GenerateCodeForComponent(QString document_path, QString component_name);
    void GenerateWorkspace(QString document_path, QString output_directory);
signals:
    void GotCodeForComponent(QString file_name, QString file_data);
    void GotJava(bool ready, QString message);
private:
    bool GenerateComponents(QString document_path, QString output_directory, QStringList component_names=QStringList(), bool toast_notify = true);
    bool GenerateDatatypes(QString document_path, QString output_directory, bool toast_notify = true);
    QStringList GetMiddlewareArgs();

    bool GotJava_();

    ProcessResult RunSaxonTransform(QString transform_path, QString document, QString output_directory, QStringList arguments=QStringList());

    ProcessRunner* runner_;
    ViewController* view_controller_;
    QString saxon_jar_path_;
    QString transforms_path_;

    QMutex mutex_;
    bool running_validation = false;
    
    bool got_java_;
};


#endif // EXECUTIONMANAGER_H
