#ifndef WE_CPU_H
#define WE_CPU_H

// Forward decl.
class WE_CPU_Impl;

class WE_CPU{
    public:
        WE_CPU();
        ~WE_CPU();

        int IntOp(double loops);
        int FloatOp(double loops);
        int Whetstone(double loops);
        int Dhrystone(double loops);
        int MWIP(double loops);
        int DMIP(double loops);

    private:
        WE_CPU_Impl * impl_;
};

#endif  //WE_CPU_H
