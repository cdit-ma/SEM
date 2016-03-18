#ifndef MODELTESTER_H
#define MODELTESTER_H
#include <QObject>
#include <QString>
class ModelTester : QObject
{
    Q_OBJECT
public:
    ModelTester();

    bool loadTest(QString model, int repeatCount = 10);

    float printMemoryUsage(QString name);

    void sleep(int ms);
    float getMemoryUsage();
signals:
    void importProjects(QStringList);
    void undo();
    void redo();
    void triggerAction(QString);
};

#endif // MODELTESTER_H
