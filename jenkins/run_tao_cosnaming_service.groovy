#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent none
    parameters{
        string(name: 'node_name', defaultValue: 'master', description: 'The name of the node to run the TAO naming service on.')
        string(name: 'port', defaultValue: '4355', description: 'The port number to run the TAO naming service on.')
    }

    stages{
        stage("Run TAO Naming Service"){
            steps{
                node(params.node_name){
                    script{
                        def endpoint = "iiop://${env.IP_ADDRESS}:${params.port}"
                        print("TAO Endpoint: ${endpoint}")

                        if(utils.runScript("${TAO_ROOT}/orbsvcs/Naming_Service/tao_cosnaming -ORBEndpoint ${endpoint}") != 0){
                            error('Running TAO naming service failed.')
                        }
                    }
                }
            }
        }
    }
}