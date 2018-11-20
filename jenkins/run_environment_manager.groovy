#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node "master"}

    parameters{
        string(name: 'ip_address', defaultValue: "${env.ENVIRONMENT_MANAGER_IP_ADDRESS}", description: 'The IP address of the interface the Environment Manager should use')
        string(name: 'port', defaultValue: '20000', description: 'The port that the Environment Manager should use')
        string(name: 'tao_ns_endpoint', defaultValue: '', description: 'The qualified endpoint of the TAO naming service that the Environment Manager should use')
        string(name: 'qpid_broker_endpoint', defaultValue: '', description: 'The qualified endpoint of the QPiD broker that the Environment Manager should use')
    }

    stages{
        stage("Run Environment Manager"){
            steps{
                script{
                    def args = "-a ${params.ip_address} "
                    args += "-r ${params.port} "
                    
                    if(params.tao_ns_endpoint){
                        args += "-t ${params.tao_ns_endpoint} "
                    }
                    if(params.qpid_broker_endpoint){
                        args += "-q ${params.qpid_broker_endpoint} "
                    }

                    if(!utils.runScript("${RE_PATH}/bin/re_environment_manager ${args}") == 0){
                        error('Running re_environment_manager failed!')
                    }
                }
            }
        }
    }
}