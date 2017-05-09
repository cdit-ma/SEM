#include <QCoreApplication>
#include "modelcontrollertests.h"
/*#include "viewcontrollertests.h"
#include "modelelementtests.h"
#include "AdoptionTests/modeladoptiontest.h"
#include "AdoptionTests/interfacedefinitionsadoptiontest.h"
*/
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    ModelControllerTests mcTests;
    /*
    ModelElementTests meTests;
    ViewControllerTests vcTests;
    ModelAdoptionTest maTest;
    InterfaceDefinitionsAdoptionTest idaTest;*/

    int results = 0;
    results += QTest::qExec(&mcTests, argc, argv);
    /*
    results += QTest::qExec(&meTests, argc, argv);
    results += QTest::qExec(&vcTests, argc, argv);
    results += QTest::qExec(&maTest, argc, argv);
    results += QTest::qExec(&idaTest, argc, argv);*/

    qCritical() << (results ? "FAIL" : "PASS");

    return results;
}

