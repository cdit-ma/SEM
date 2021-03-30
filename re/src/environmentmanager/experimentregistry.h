//
// Created by mitchell on 24/3/21.
//

#ifndef SEM_EXPERIMENTREGISTRY_H
#define SEM_EXPERIMENTREGISTRY_H

class ExperimentRegistry {
public:
    ExperimentRegistry();
private:
    std::vector<std::unique_ptr<ExperimentManager>> experiment_managers_;
    Environment environment_;

};

#endif // SEM_EXPERIMENTREGISTRY_H
