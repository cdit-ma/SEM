#ifndef WE_CPU_H
#define WE_CPU_H

#include <core/worker.h>

// Forward decl.
class WE_CPU_Impl;

class WE_CPU : public Worker{
    public:
        WE_CPU(Component* component, std::string inst_name);
        ~WE_CPU();

        int IntOp(double loops);
        int FloatOp(double loops);
        int Whetstone(double loops);
        int Dhrystone(double loops);
        int MWIP(double loops);
        int DMIP(double loops);
        int MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
                        std::vector<float> &matrixC);

    private:
        WE_CPU_Impl * impl_;
};

#endif  //WE_CPU_H
