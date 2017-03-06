#include "WE_CPU_Impl.h"

#include <iostream>
#include <cmath>      // Math. functions needed for whets.cpp?

#include "intop.cpp"
#include "floatop.cpp"
#include "whets.cpp"
#include "dhry_1.cpp"

int WE_CPU_Impl::IntOp(double loops) {
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            integerop workload;
            workload.loop((unsigned long long)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::IntOp Loops: " << loops << std::endl;
    }
    return -1;
}

int WE_CPU_Impl::FloatOp(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            floatop workload;
            workload.loop((unsigned long long)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::FloatOp Loops: " << loops << std::endl;
    }
    return -1;
}

int WE_CPU_Impl::Whetstone(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            whetstone workload;
            workload.loop((int)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::Whetstone Loops: " << loops << std::endl;
    }
    return -1;
}

int WE_CPU_Impl::Dhrystone(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            dhrystone workload;
            workload.loop((int)loops);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::Dhrystone Loops: " << loops << std::endl;
    }
    return -1;
}

int WE_CPU_Impl::MWIP(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }
        // Workload Execution.
        if(loops > 0){
            whetstone workload;
            workload.mwip((int)loops, true);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::MWIP Loops: " << loops << std::endl;
    }
    return -1;
}

int WE_CPU_Impl::DMIP(double loops){
    try{
        // This guard will ensure only one CPU Workload Execution is occurring at any given time
        //  i.e. for this worker in this component.
        std::unique_lock<std::mutex> guard(lock_, std::try_to_lock);
        if(!guard.owns_lock()){
            return -1;
        }

        // Workload Execution.
        if(loops > 0){
            dhrystone workload;
            workload.dmip((int)loops, true);
        }
        return 0;
    }
    catch(...){
        std::cerr << "Caught unkown exception WE_CPU_Impl::MWIP Loops: " << loops << std::endl;
    }
    return -1;
}
