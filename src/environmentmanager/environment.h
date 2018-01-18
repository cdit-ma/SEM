#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

class Environment{

    public:
        AssignPort();
        UnassignPort();


    private:
        std::set<int> available_ports_;
        

};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT