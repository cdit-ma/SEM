#include "WE_CPU.h"
#include "WE_CPU_Impl.h"

WE_CPU::WE_CPU(){
    impl_ = new WE_CPU_Impl();
}

WE_CPU::~WE_CPU(){
    if(impl_){
        delete impl_;
        impl_ = 0;
    }
}

int WE_CPU::IntOp(double loop){
    return impl_->IntOp(loop);
}

int WE_CPU::FloatOp(double loop){
    return impl_->FloatOp(loop);
}

int WE_CPU::Whetstone(double loop){
    return impl_->Whetstone(loop);
}

int WE_CPU::Dhrystone(double loop){
    return impl_->Dhrystone(loop);
}

int WE_CPU::MWIP(double loop){
    return impl_->MWIP(loop);
}

int WE_CPU::DMIP(double loop){
    return impl_->DMIP(loop);
}
