#ifndef INTERFACEDEFINITIONSADOPTIONTEST_H
#define INTERFACEDEFINITIONSADOPTIONTEST_H

#include <QObject>
#include <QtTest/QtTest>

#include "../src/Controllers/modelcontroller.h"
class InterfaceDefinitionsAdoptionTest : public QObject{
    Q_OBJECT
public:

private slots:
    void test();
    void portTest();
    void test_data();
};

#endif // INTERFACEDEFINITIONSADOPTIONTEST_H
