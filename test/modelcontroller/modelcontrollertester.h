#ifndef MODELCONTROLLERTESTER_H
#define MODELCONTROLLERTESTER_H

#include <QSharedPointer>
#include "../../src/modelcontroller/modelcontroller.h"
class EntityFactoryRegistryBroker;
class ModelControllerTester: public QObject{
    Q_OBJECT
    
    private slots:
        void initTestCase();
        void init();
        void cleanup();

        void test_models();
        void test_models_data();
    private:
        bool try_open_rel_graphml(const QString rel_model_path);
        bool try_open_graphml(const QString model_path);

        QString test_dir;
        QSharedPointer<ModelController> controller;
};


#endif //MODELCONTROLLERTESTER_H
