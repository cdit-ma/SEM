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

        void test_multiple_import();
        void test_no_file_import();
    private:
        bool try_import_idls(const QStringList idl_paths);
        QPair<bool, QString> ParseIdls(const QStringList idl_paths);
        
        QString test_dir;
        QSharedPointer<ModelController> controller;
};

#endif //IDLPARSERTESTER_H
