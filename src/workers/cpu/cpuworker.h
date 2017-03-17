#ifndef WORKERS_CPU_CPUWORKER_H
#define WORKERS_CPU_CPUWORKER_H

#include <core/worker.h>

// Forward decl.
class CpuWorker_Impl;

class CpuWorker : public Worker{
    public:
        CpuWorker(Component* component, std::string inst_name);
        ~CpuWorker();

        int IntOp(double loops);
        int FloatOp(double loops);
        int Whetstone(double loops);
        int Dhrystone(double loops);
        int MWIP(double loops);
        int DMIP(double loops);
        int MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
                        std::vector<float> &matrixC);

    private:
        CpuWorker_Impl * impl_;
};

#endif  //WORKERS_CPU_CPUWORKER_H
