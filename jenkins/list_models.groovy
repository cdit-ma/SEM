#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node{"re"}}
    parameters{
        string(name: 'environment_manager_address', defaultValue: "${env.ENVIRONMENT_MANAGER_ADDRESS}", description: 'The address of the Environment Manager to use for this experiment')
    }

    stages{
        stage("List Experiments"){
            steps{
                script{
                    def args = "-l "
                    args += "-e ${params.environment_manager_address} "
                    if(!utils.runReEnvironmentController(args)){
                        error('Listing experiments failed.')
                    }
                }
            }
        }
    }
}