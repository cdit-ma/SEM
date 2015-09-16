#include "process.h"

Process::Process():BehaviourNode(true, false, false){}

Process::~Process(){}

bool Process::canConnect(Node*)
{
    return false;
}

bool Process::canAdoptChild(Node*)
{
    return false;
}
