#ifndef IDLPARSERTESTER_H
#define IDLPARSERTESTER_H

#include <QSharedPointer>
#include "../../src/modelcontroller/modelcontroller.h"
class IdlParserTester: public QObject{
    Q_OBJECT
    private slots:
        void initTestCase();
        void init();
        void cleanup();

        void test_idl_import_data();
        void test_idl_import();
    private:
        bool try_import_idl(const QString idl_path);
        QPair<bool, QString> ParseIdl(const QString idl_path);
        
        QString test_dir;
        QSharedPointer<ModelController> controller;
};

#endif //IDLPARSERTESTER_H
