#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

final DOCKER_REGISTRY_ENDPOINT = env.DOCKER_REGISTRY_ENDPOINT
def deploy_map = [:]

pipeline{
    agent none
    parameters{
        string(name: 're_base_image_name', defaultValue: '', description: 'Docker base image name (full_deps if other dependencies are not needed)')
        booleanParam(name: 'rebuild_re_image', defaultValue: true, description: 'Rebuild re image')
        booleanParam(name: 'rebuild_logan_image', defaultValue: true, description: 'Rebuild logan image')
    }
    stages{
        stage('Build docker images') {
            steps{
                script{
                    node("docker_runtime") {
                        def base_image_arg = ""
                        def base_image_name = "full_deps"
                        if(params.re_base_image_name) {
                            base_image_arg = "--build-arg base_image=${DOCKER_REGISTRY_ENDPOINT}/${params.re_base_image_name}"
                            base_image_name = "${params.re_base_image_name}"
                        }
                        def base_image_registry_name = "${DOCKER_REGISTRY_ENDPOINT}/${base_image_name}"
                        
                        if(params.rebuild_re_image){
                            dir("${HOME}/re") {
                                if(utils.runScript("docker build -t re_full -f docker/re_full/Dockerfile ${base_image_arg} .") != 0) {
                                    error("Failed to build re_full image")
                                }
                                if(utils.runScript("docker tag re_full ${DOCKER_REGISTRY_ENDPOINT}/re_full") != 0) {
                                    error("Failed to tag re_full image")
                                }
                                if(utils.runScript("docker push ${DOCKER_REGISTRY_ENDPOINT}/re_full") != 0) {
                                    error("Failed to push re_full image after build")
                                }
                            }
                        }
                        if(params.rebuild_logan_image) {
                            dir("${HOME}/re") {
                                if(utils.runScript("docker build -t logan -f logan/docker/aggregation_server/Dockerfile ${base_image_arg} .") != 0) {
                                    error("Failed to build logan image")
                                }
                                if(utils.runScript("docker tag logan ${DOCKER_REGISTRY_ENDPOINT}/logan") != 0) {
                                    error("Failed to tag logan image")
                                }
                                if(utils.runScript("docker push ${DOCKER_REGISTRY_ENDPOINT}/logan") != 0) {
                                    error("Failed to push logan image after build")
                                }
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
                                if(utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/re_full") != 0){
                                    error("Failed to pull re_full docker image on ${node_name}")
                                }

                                if(utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/logan") != 0) {
                                    error("Failed to pull logan docker image on ${node_name}")
                                }
                            }
                        }
                    }
                    parallel deploy_map
                }
            }
        }
    }
}
