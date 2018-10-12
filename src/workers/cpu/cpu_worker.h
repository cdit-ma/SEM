#ifndef WORKERS_CPU_CPUWORKER_H
#define WORKERS_CPU_CPUWORKER_H

#include <memory>
#include <core/worker.h>

class Cpu_Worker_Impl;
class Cpu_Worker : public Worker{
    public:
        Cpu_Worker(const BehaviourContainer& container, const std::string& inst_name);
        ~Cpu_Worker();

        int IntOp(double loops);
        int FloatOp(double loops);
        int Whetstone(double loops);
        int Dhrystone(double loops);
        int MWIP(double loops);
        int DMIP(double loops);
        int MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB, std::vector<float> &matrixC);
    private:
        std::unique_ptr<Cpu_Worker_Impl> impl_;
};

#endif  //WORKERS_CPU_CPUWORKER_H
