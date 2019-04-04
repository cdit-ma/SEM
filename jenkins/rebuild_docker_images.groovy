#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

final DOCKER_REGISTRY_ENDPOINT = env.DOCKER_REGISTRY_ENDPOINT
def deploy_map = [:]

pipeline{
    agent none
    parameters{
        string(name: 're_base_image_name', defaultValue: '', description: 'Docker base image name')
        booleanParam(name: 'rebuild_re_image', defaultValue: true, description: 'Rebuild re image')
        booleanParam(name: 'rebuild_logan_image', defaultValue: true, description: 'Rebuild logan image')
    }
    stages{
        stage('Build docker images') {
            steps{
                script{
                    node("docker_runtime") {
                        if(params.rebuild_re_image){
                            def base_image_arg = ""
                            if(params.re_base_image_name) {
                                base_image_arg = "--build-arg base_image=${params.re_base_image_name}"
                            }
                            dir("${HOME}/re") {
                                // Rebuild docker image
                                utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/${params.re_base_image_name}")
                                utils.runScript("docker build -t re_full -f docker/re_full/Dockerfile ${base_image_arg} .")
                                utils.runScript("docker tag re_full ${DOCKER_REGISTRY_ENDPOINT}/re_full")
                                utils.runScript("docker push ${DOCKER_REGISTRY_ENDPOINT}/re_full")
                            }
                        }
                        if(params.rebuild_logan_image) {
                            dir("${HOME}/re/logan") {
                                // Rebuild docker image
                                utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/logan")
                                utils.runScript("docker build -t logan -f docker/aggregation_server/Dockerfile .")
                                utils.runScript("docker tag logan ${DOCKER_REGISTRY_ENDPOINT}/logan")
                                utils.runScript("docker push ${DOCKER_REGISTRY_ENDPOINT}/logan")
                            }
                        }
                    }
                }
            }
        }
        stage('Deploy docker images'){
            steps{
                script{
                    for(def n in nodesByLabel('docker_runtime')){
                        def node_name = n
                        if(node_name == ''){
                            node_name = 'master'
                        }

                        deploy_map[node_name] = {
                            node(node_name){
                                utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/re_full")
                                utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/logan")
                            }
                        }
                    }
                    parallel deploy_map
                }
            }
        }
    }
}
