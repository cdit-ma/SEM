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

private:
    void actionCompleted(bool success, QString result);
    bool actionPassed;
    QString actionResult;


};

#endif //MODELCONTROLLERTESTS_H
