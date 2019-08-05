#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node "aggregation_broker"}

    parameters{
        string(name: 'broker_port', defaultValue: "12345", description: 'Broker -> MEDEA interface port')
        string(name: 'db_ip_address', defaultValue: "${env.DOCKER_IP_ADDRESS}", description: 'The IP address of the database')
        string(name: 'db_password', defaultValue: '', description: 'The password for the database')
    }

    stages{
        stage("Run Aggregation Broker"){
            steps{
                script{
                    def docker_registry_endpoint = "${env.DOCKER_REGISTRY_ENDPOINT}"

                    if(!"${env.IP_ADDRESS}") {
                        error('IP_ADDRESS env var not set on node')
                    }
                    
                    print("Aggregation broker address: ${env.IP_ADDRESS}")
                    def args = "-b tcp://${env.IP_ADDRESS}:${params.broker_port} "

                    if(params.db_ip_address){
                        args += "-i ${params.db_ip_address} "
                    }

                    if(params.db_password){
                        args += "-p ${params.db_password} "
                    }

                    if (docker_registry_endpoint) {
                        docker.image("${docker_registry_endpoint}/logan").inside("--network host") {
                            if(!utils.runScript("/logan/bin/aggregation_broker ${args}") == 0){
                                error('Running aggregation broker failed!')
                            }
                        }
                    } else {
                        error("Docker registry endpoint not set")
                    }
                }
            }
        }
    }
}