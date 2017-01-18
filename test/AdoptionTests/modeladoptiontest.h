#ifndef MODELADOPTIONTEST_H
#define MODELADOPTIONTEST_H

#include <QObject>
#include <QtTest/QtTest>

#include "../src/Controllers/modelcontroller.h"

class ModelAdoptionTest : public QObject{
    Q_OBJECT
public:

private slots:
    void test();
    void test_data();
};

#endif // MODELADOPTIONTEST_H
