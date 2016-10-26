#include <QTest>
#include <QtTest>
#include <QCoreApplication>
#include "modelcontrollertests.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    ModelControllerTests mcTests;

    int results = 0;
    results += QTest::qExec(&mcTests, argc, argv);
    return results;
}

