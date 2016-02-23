#include "modeltester.h"
#include "Controller/controller.h"
#include <QDebug>
#include <QTime>
#ifdef _WIN32
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#endif



ModelTester::ModelTester()
{

    qCritical() << "Starting in 10 seconds";
    sleep(1000);

    float initialMemory = getMemoryUsage();
    qCritical() << "Memory Usage on Load: " << initialMemory << "KB.";

    NewController* controller = new NewController();
    controller->initializeModel();
    connect(this, SIGNAL(importProjects(QStringList)), controller, SLOT(importProjects(QStringList)));

    float postInitializeMemory = getMemoryUsage();
    qCritical() << "Memory Usage after Initialize: " << postInitializeMemory << "KB.";

    QFile file("e:/MCMS.graphml");

    qCritical() << "FILE";

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();
 qCritical() << "FILE";
    int repeatCount = 1;
    int loadCount = 1;
    QTime  time;
    time.start();
 qCritical() << "FILE";
    //sleep(5);
    //float priorMemory = getMemoryUsage();
    //sleep(5);

     qCritical() << "FILE";
    for(int j = 0 ; j < repeatCount; j++){
        float beforeLoad = getMemoryUsage();
        //if(beforeLoad > priorMemory){
        //    qCritical() << "Delta Memory: " << beforeLoad - priorMemory << "KB.";
        //}

        for(int i = 0 ; i < loadCount; i++){
            QStringList list;
            list << xmlText;
            emit importProjects(list);

        }
        //sleep(10);
        //qCritical() << time.elapsed();
        //float afterLoad = getMemoryUsage();
        //qCritical() << "Memory Usage After Load: " << afterLoad << "KB.";
    }

    qCritical() << "LOADED FILE:";

    delete controller;
    qCritical() << " DELETED YO:";
}

void ModelTester::sleep(int ms){
    #ifdef _WIN32
        Sleep(ms);
    #else
        sleep(ms);
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
