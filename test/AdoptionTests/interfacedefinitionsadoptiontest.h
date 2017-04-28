#ifndef INTERFACEDEFINITIONSADOPTIONTEST_H
#define INTERFACEDEFINITIONSADOPTIONTEST_H

#include <QObject>
#include <QtTest/QtTest>


class InterfaceDefinitionsAdoptionTest : public QObject{
    Q_OBJECT
public:

private slots:
    void test();
    void portTest();
    void test_data();
};

#endif // INTERFACEDEFINITIONSADOPTIONTEST_H
