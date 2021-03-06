#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node params.node_name}

    parameters{
        string(name: 'node_name', defaultValue: 'master', description: 'The name of the node to run the Qpid broker on.')
        string(name: 'port', defaultValue: '5672', description: 'The port that the Qpid broker should use.')
        string(name: 'broker_args', defaultValue: '', description: 'The arguments to pass to Qpid.')
    }
    

    stages{
        stage("Run Qpid Broker"){
            steps{
                script{
                    def endpoint = "${env.IP_ADDRESS}:${params.port}"
                    print("Qpid Broker Endpoint: ${endpoint}")

                    if(utils.runScript("${env.QPID_ROOT}/sbin/qpidd -p ${params.port} ${params.broker_args}") != 0){
                        error('Running Qpid broker failed.')
                    }
                }
            }
        }
    }
}
