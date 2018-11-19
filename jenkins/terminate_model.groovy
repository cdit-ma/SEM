#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node "re"}

    parameters{
        string(name: 'experiment_name', defaultValue: '', description: 'The name for the experiment')
        string(name: 'environment_manager_address', defaultValue: "${env.ENVIRONMENT_MANAGER_ADDRESS}", description: 'The address of the Environment Manager to use for this experiment')
        booleanParam(name: 'is_regex', defaultValue: false, description: 'Should the Environment Manager treat the experiment_name field as a regex field.')
    }

    stages{
        stage("Terminate Experiment"){
            steps{
                script{
                    def args = "-s -n \"${params.experiment_name}\" "
                    args += "-e ${params.environment_manager_address} "
                    if(params.is_regex){
                        args += "-r "
                    }
                    if(!utils.runReEnvironmentController(args)){
                        error('Termination of experiment failed.')
                    }
                }
            }
        }
    }
}