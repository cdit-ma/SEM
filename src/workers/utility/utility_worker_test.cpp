#include "utility_worker.h"
#include <iostream>
#include <thread>
#include <chrono>
int main(){
    auto c = std::make_shared<Component>();
    Utility_Worker ute(c, "ute");
    std::cout << std::fixed; std::cout.precision(8);
    auto time_str = ute.GetTimeOfDayString();
    auto time_dbl = ute.GetTimeOfDay();
    std::cout << "Utility_Worker::GetTimeOfDay(): " << time_dbl << std::endl;
    std::cout << "Utility_Worker::GetTimeOfDayString(): " << time_str << std::endl;
    std::cout << std::endl;
    std::cout << "Utility_Worker::UUID(): " << ute.GenerateUUID() << std::endl;
    std::cout << "Utility_Worker::UUID(): " << ute.GenerateUUID() << std::endl;
    std::cout << std::endl;

    {
        auto t1 = ute.GetTimeOfDay();
        ute.Sleep(1);
        auto t2 = ute.GetTimeOfDay();
        std::cout << "Utility_Worker::Sleep(1): " << t2 - t1 << std::endl;
    }

    {
        auto t1 = ute.GetTimeOfDay();
        ute.USleep(1000);
        auto t2 = ute.GetTimeOfDay();
        std::cout << "Utility_Worker::USleep(1000): " << t2 - t1 << std::endl;
    }
    std::cout << std::endl;

    double i = 1;
    double n = 10;
    double b = 10;

    std::cout << "Utility_Worker::EvaluateComplexity(\"cos(13*i*(2*pi)/L)\", i=1, L=64): " << ute.EvaluateComplexity("cos(13*i*(2*pi)/L)", 1.0, 64.0) << std::endl;

    const int nrolls = 1000000;
    const int nstars = 100;
    const int nintervals = 10;

    int rint[nintervals]={};
    int rreal[nintervals]={};
    int rexpr[nintervals]={};
    int rnorm[nintervals]={};
    
    std::cout << "Utility_Worker::SetRandomSeed(1): " << std::endl;
    ute.SetRandomSeed(1);
      
    for(int i = 0; i < nrolls; ++i){
        auto rint_ = ute.RandomUniformInt(0, 9);
        auto rreal_ = ute.RandomUniformReal(0.0, 1.0);
        auto rexpr_ = ute.RandomExponentialReal(3.5);
        auto rnorm_ = ute.RandomNormalReal(5.0, 2.0);
        rint[rint_]++;
        rreal[int(nintervals*rreal_)]++;
        if(rnorm_ > 0.0 && rnorm_ < 10.0){
            rnorm[int(rnorm_)]++;
        }

        if(rexpr_ < 1.0){
            rexpr[int(nintervals*rexpr_)]++;
        }
    }

    
    
    std::cout << std::endl << "Utility_Worker::RandomUniformInt(0, 9):" << std::endl;
    for(int i = 0; i < nintervals; i++){
        std::cout << "\t" << i << ": " << std::string(rint[i]*nstars/nrolls,'*') << std::endl;
    }

    std::cout << std::endl << "Utility_Worker::RandomUniformReal(0.0, 1.0):" << std::endl;
    for(int i = 0; i < nintervals; i++){
        std::cout << "\t" << i << ": " << std::string(rreal[i]*nstars/nrolls,'*') << std::endl;
    }

    std::cout << std::endl << "Utility_Worker::RandomNormalReal(5.0, 2.0):" << std::endl;
    for(int i = 0; i < nintervals; i++){
        std::cout << "\t"  << i << ": " << std::string(rnorm[i]*nstars/nrolls,'*') << std::endl;
    }

    std::cout << std::endl << "Utility_Worker::RandomExponentialReal(3.5):" << std::endl;
    std::cout << std::fixed; std::cout.precision(1);
  
    for(int i = 0; i < nintervals; i++){
      std::cout << "\t" << float(i)/nintervals << "-" << float(i+1)/nintervals << ": ";
      std::cout << std::string(rexpr[i]*nstars/nrolls,'*') << std::endl;
    }

    return 0;

}
