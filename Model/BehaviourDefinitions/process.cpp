#include "process.h"

Process::Process():BehaviourNode(false, false, false){
    setUnconnectable(false);
}

Process::~Process(){}

bool Process::canConnect(Node*)
{
    return false;
}

bool Process::canAdoptChild(Node*)
{
    return false;
}
