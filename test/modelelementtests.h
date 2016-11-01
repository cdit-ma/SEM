#ifndef MODELELEMENTTESTS_H
#define MODELELEMENTTESTS_H

#include <QObject>
#include <QtTest/QtTest>


#include "../src/Controllers/modelcontroller.h"

class ModelElementTests : public QObject{
    Q_OBJECT
public:

signals:

public slots:

private slots:
    void nodeTest();
    void modelCreationTest();
    void componentImplTest();

private:
    Model* setupModel();
};

#endif // MODELELEMENTTESTS_H
