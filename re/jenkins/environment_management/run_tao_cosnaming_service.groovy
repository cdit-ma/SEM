#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node params.node_name}
    
    parameters{
        string(name: 'node_name', defaultValue: 'master', description: 'The name of the node to run the TAO naming service on.')
        string(name: 'port', defaultValue: '4355', description: 'The port number to run the TAO naming service on.')
    }

    stages{
        stage("Run TAO Naming Service"){
            steps{
                script{
                    def endpoint = "iiop://${env.IP_ADDRESS}:${params.port}"
                    print("TAO Endpoint: corbaloc:iiop:${env.IP_ADDRESS}:${params.port}")

                    if(utils.runScript("${env.TAO_ROOT}/orbsvcs/Naming_Service/tao_cosnaming -ORBEndpoint ${endpoint}") != 0){
                        error('Running TAO naming service failed.')
                    }
                }
            }
        }
    }
}