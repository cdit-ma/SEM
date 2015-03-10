#include "modeltester.h"
#include "Controller/newcontroller.h"
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

    QFile file("C:/newVUAV.graphml");

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();

    int repeatCount = 10;
    int loadCount = 2;

      sleep(20);

    float priorMemory = getMemoryUsage();
    float previousGrowth = 0;
    for(int j = 0 ; j < repeatCount; j++){
        float beforeLoad = getMemoryUsage();
        if(beforeLoad > priorMemory){
            qCritical() << "Delta Memory: " << beforeLoad - priorMemory << "KB.";
        }

        //controller->view_TriggerAction("Loading GraphML");
        for(int i = 0 ; i < loadCount; i++){
            //controller->view_Paste(xmlText);
        }

        float afterLoad = getMemoryUsage();
        //qCritical() << "Memory Usage After Load: " << afterLoad << "KB.";

        //controller->view_Undo();

        //controller->view_ClearModel();
        sleep(20);
        float afterClear = getMemoryUsage();

        //qCritical() << "Memory Usage After Clear: " << afterClear << "KB.";
    }


    xmlText = "";

    delete controller;
    sleep(1);
    float afterDelete = getMemoryUsage();
    qCritical() << "Memory Usage After Delete: " << afterDelete << "KB.";
    qCritical() << "Total Memory Growth: " << afterDelete-initialMemory << "KB.";

}

void ModelTester::sleep(int ms){
    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms*1000);
#endif
}

float ModelTester::getMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS memCounter;
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof( memCounter ));
    if(result){
        return memCounter.WorkingSetSize / 1024.0;
    }
    return 0;
}
