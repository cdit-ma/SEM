#ifndef WORKERS_CPU_CPUWORKER_H
#define WORKERS_CPU_CPUWORKER_H

#include <memory>
#include "worker.h"

class Cpu_Worker_Impl;
class Cpu_Worker : public Worker{
    public:
        Cpu_Worker(const BehaviourContainer& container, const std::string& inst_name);
        ~Cpu_Worker();

        const std::string& get_version() const override;

        int IntOp(double loops, int thread_count = 1, bool split_workload = true);
        int FloatOp(double loops, int thread_count = 1, bool split_workload = true);
        int Whetstone(double loops, int thread_count = 1, bool split_workload = true);
        int Dhrystone(double loops, int thread_count = 1, bool split_workload = true);
        int MWIP(double loops, int thread_count = 1, bool split_workload = true);
        int DMIP(double loops, int thread_count = 1, bool split_workload = true);
        int MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB, std::vector<float> &matrixC);
    private:
        int RunParallel(std::function<int (double)> fn, std::string name, double loops, int thread_count, bool split_workload);

        std::unique_ptr<Cpu_Worker_Impl> impl_;
};

#endif  //WORKERS_CPU_CPUWORKER_H
