#ifndef MODELCONTROLLERTESTS_H
#define MODELCONTROLLERTESTS_H

#include <QString>
#include <QtTest/QtTest>

class ModelControllerTests: public QObject{
    Q_OBJECT

public:
    ModelControllerTests(){};

signals:
    void setup_controller(QString file_path="");
    void teardown_controller();
    void open_project(QString file_name, QString file_data);
private slots:
    void test_controller();
    void load_helloworld();
};

#endif //MODELCONTROLLERTESTS_H
