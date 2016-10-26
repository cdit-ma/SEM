#ifndef MODELCONTROLLERTESTS_H
#define MODELCONTROLLERTESTS_H

#include <QtTest/QtTest>

class ModelControllerTests: public QObject{
    Q_OBJECT

public:

signals:
    void setupController();
    void teardownController();
    void openProject(QString fileName, QString data);

private slots:
    //Tests
    void initializeControllerTest();
    void loadHelloWorldTest();
};

#endif //MODELCONTROLLERTESTS_H
