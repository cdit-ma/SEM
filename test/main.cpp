#include <QTest>
#include <QtTest>
#include <QCoreApplication>
#include "modelcontrollertests.h"
#include "viewcontrollertests.h"
#include "modelelementtests.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    ModelControllerTests mcTests;
    ModelElementTests meTests;
    ViewControllerTests vcTests;

    int results = 0;
    results += QTest::qExec(&mcTests, argc, argv);
    results += QTest::qExec(&meTests, argc, argv);
    results += QTest::qExec(&vcTests, argc, argv);

    return results;
}

