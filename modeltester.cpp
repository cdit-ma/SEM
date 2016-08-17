#include "modeltester.h"
#include "Controller/controller.h"
#include <QDebug>
#include <QTime>
#include <QStringBuilder>
#ifdef _WIN32
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#endif



ModelTester::ModelTester()
{
}

bool ModelTester::loadTest(QString model, int repeatCount)
{
    printMemoryUsage("Initial");

    NewController* controller = new NewController();
    controller->setupController();
    connect(this, SIGNAL(importProjects(QStringList)), controller, SLOT(importProjects(QStringList)));
    connect(this, SIGNAL(undo()), controller, SLOT(undo()));
    connect(this, SIGNAL(redo()), controller, SLOT(redo()));
    connect(this, SIGNAL(triggerAction(QString)), controller, SLOT(triggerAction(QString)));

    printMemoryUsage("Post controller setup");

    //qCritical() << controller->getModel()->getChildren().size();
    QFile file(model);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
        delete controller;
        return false;
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();

    emit triggerAction("LOADING FILE");


    int initialComponentCount = 0;//controller->getModel()->getChildren().size();

    for(int i = 0 ; i < repeatCount; i++){
        qint64 loadStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
        emit triggerAction("Loading File #" % QString::number(i));
        QStringList list;
        list << xmlText;
        emit importProjects(list);
        int componentAfterLoad = 0;//controller->getModel()->getChildren().size();
        qint64 loadFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qCritical() << "Loaded " << componentAfterLoad << " Components in: " << loadFinish - loadStart << "MS.";
        emit undo();

        qint64 undoFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qCritical() << "Undone " << componentAfterLoad << " Components in: " << undoFinish - loadFinish << "MS.";

        int componentAfterUndo = 0;//controller->getModel()->getChildren().size();
        if(componentAfterUndo != initialComponentCount){
            qCritical() << "Undo didn't restore state! Failure";
            return false;
        }
        sleep(1000);
        printMemoryUsage("After Load #" % QString::number(i));
    }

    //THEN DELETE.
    delete controller;

    sleep(1000);
    printMemoryUsage("After Deletion");

    return true;
}

float ModelTester::printMemoryUsage(QString name)
{
    float memoryUsage = getMemoryUsage();
    qCritical() << "Memory Usage - " << name << " = " << memoryUsage << "KB";
    return memoryUsage;
}

void ModelTester::sleep(int ms){
    #ifdef _WIN32
        Sleep(ms);
    #else
    return;
    sleep(ms / 1000);
#endif
}

float ModelTester::getMemoryUsage()
{
    #ifdef _WIN32
    PROCESS_MEMORY_COUNTERS memCounter;
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof( memCounter ));
    if(result){
        return memCounter.WorkingSetSize / 1024.0;
    }

#endif
    return 0;

}
