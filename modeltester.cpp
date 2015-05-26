#include "modeltester.h"
#include "Controller/controller.h"
#include <QDebug>
#ifdef _WIN32
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#endif



ModelTester::ModelTester()
{
    float initialMemory = getMemoryUsage();
    qCritical() << "Memory Usage on Load: " << initialMemory << "KB.";

    NewController* controller = new NewController();
    controller->initializeModel();

    float postInitializeMemory = getMemoryUsage();
    qCritical() << "Memory Usage after Initialize: " << postInitializeMemory << "KB.";

    QFile file("C:/Users/QT5/Desktop/Models/vUAVmedea.graphML");

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();

    int repeatCount = 1;
    int loadCount = 1;


    sleep(5);
    float priorMemory = getMemoryUsage();
    float previousGrowth = 0;
    sleep(5);
    for(int j = 0 ; j < repeatCount; j++){
        float beforeLoad = getMemoryUsage();
        if(beforeLoad > priorMemory){
            qCritical() << "Delta Memory: " << beforeLoad - priorMemory << "KB.";
        }

        //controller->view_TriggerAction("Loading GraphML");
        for(int i = 0 ; i < loadCount; i++){
            controller->importProjects(QStringList(xmlText));
            //controller->view_Paste(xmlText);
        }

        float afterLoad = getMemoryUsage();
        qCritical() << "Memory Usage After Load: " << afterLoad << "KB.";

        //controller->view_Undo();

        controller->clearModel();
        sleep(20);
        float afterClear = getMemoryUsage();

        qCritical() << "Memory Usage After Clear: " << afterClear << "KB.";
    }


    xmlText = "";

    //delete controller;
    //sleep(10);
    //float afterDelete = getMemoryUsage();
    //qCritical() << "Memory Usage After Delete: " << afterDelete << "KB.";
   // qCritical() << "Total Memory Growth: " << afterDelete-initialMemory << "KB.";

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
