#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node "aggregation_server"}

    parameters{
        string(name: 'environment_manager', defaultValue: "${env.ENVIRONMENT_MANAGER_ADDRESS}", description: 'The qualified endpoint of the Environment Manager')
        string(name: 'db_address', defaultValue: '', description: 'IP address of node database is running on')
        string(name: 'db_password', defaultValue: '', description: 'The password for the database')
    }

    stages{
        stage("Run Aggregation Server"){
            steps{
                script{
                    def docker_registry_endpoint = "${env.DOCKER_REGISTRY_ENDPOINT}"

                    if(!params.db_address) {
                        error("DB address not set")
                    }

                    def args = "-i ${params.db_address} "
                    
                    if(params.environment_manager){
                        args += "-e ${params.environment_manager} "
                    }
                    if(params.db_password){
                        args += "-p ${params.db_password} "
                    }

                    if (docker_registry_endpoint) {
                        docker.image("${docker_registry_endpoint}/logan").inside("--network host") {
                            if (utils.runScript("/re/bin/aggregation_server ${args}") != 0) {
                                error('Running aggregation server failed on node: ${node_name} : ${container_id}')
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