#pragma once

#include <QObject>
#include <QString>

class VersionKeyTests: public QObject{
    Q_OBJECT
    private slots:
        void Test_MEDEAVersion();
        void Test_Comparators();
        void Test_Comparators_data();
        
        void Test_Validity();
        void Test_Validity_data();
};