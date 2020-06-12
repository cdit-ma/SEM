#ifndef WORKERS_CPU_CPUWORKER_IMPL_H
#define WORKERS_CPU_CPUWORKER_IMPL_H

#include <mutex>

class Cpu_Worker_Impl{
    public:

        /**
        * Start Workload Execution of cpu benchmark loops
        *
        * @param[in]     loops           Number or loops to execute of this benchmark
        */
        int IntOp(double loops);
        int FloatOp(double loops);
        int Whetstone(double loops);
        int Dhrystone(double loops);
        int MWIP(double loops);
        int DMIP(double loops);
        int MatrixMult(unsigned int lenA, unsigned int lenB, unsigned int lenC,
					   const float* dataA, const float* dataB, float* dataC);
    
    private:
        std::mutex lock_;
};

#endif  //WORKERS_CPU_CPUWORKER_IMPL_H
