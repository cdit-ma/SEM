//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: IP_ADDRESS
// -String parameter: PORT
stage("Run QPID Broker"){
    def node_ = env.NODE;
    def broker_options = ""

    if(env.BROKER_OPTIONS){
        broker_options = env.BROKER_OPTIONS
    }

    
    node(node_){
        def qpid_root = env.QPID_ROOT;

        if (qpid_root){
            sh qpid_root + "/sbin/qpidd " + broker_options
        }else{
            print("Missing Parameters/Environment Variables")
        }
    }
}

#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent none
    parameters{
        string(name: 'node_name', defaultValue: 'master', description: 'The name of the node to run the QPiD broker on.')
        string(name: 'port', defaultValue: '5672', description: 'The port that the QPiD broker should use.')
        string(name: 'broker_args', defaultValue: '', description: 'The arguments to pass to QPID.')
    }

    stages{
        stage("Run QPID Broker"){
            steps{
                node(params.node_name){
                    script{
                        def endpoint = "tcp://${env.IP_ADDRESS}:${params.port}"
                        print("QPID Broker Endpoint: ${endpoint}")

                        if(utils.runScript("${QPID_ROOT}/sbin/qpidd -p ${params.port} ${params.broker_args}") != 0){
                            error('Running QPID broker failed.')
                        }
                    }
                }
            }
        }
    }
}
