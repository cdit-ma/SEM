#ifndef MODELTESTER_H
#define MODELTESTER_H
#include <QObject>
#include <QString>
class ModelTester : QObject
{
    Q_OBJECT
public:
    ModelTester();

    void sleep(int ms);
    float getMemoryUsage();
signals:
    void importProjects(QStringList);
};

#endif // MODELTESTER_H
