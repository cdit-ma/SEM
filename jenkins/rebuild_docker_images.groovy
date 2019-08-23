#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

final DOCKER_REGISTRY_ENDPOINT = env.DOCKER_REGISTRY_ENDPOINT
def deploy_map = [:]

pipeline{
    agent none
    parameters{
        string(name: 're_base_image_name', defaultValue: '', description: 'Docker base image name (cdit-ma/full_deps if other dependencies are not needed)')
    }
    stages{
        stage('Build docker images') {
            steps{
                script{
                    node("docker_runtime") {
                        def base_image_arg = ""
                        def base_image_name = "cdit-ma/full_deps"
                        if(params.re_base_image_name) {
                            base_image_arg = "--build-arg base_image=${DOCKER_REGISTRY_ENDPOINT}/${params.re_base_image_name}"
                            base_image_name = "${params.re_base_image_name}"
                        }
                        def base_image_registry_name = "${DOCKER_REGISTRY_ENDPOINT}/${base_image_name}"
                        
                        if(params.rebuild_re_image){
                            dir("${HOME}/re") {
                                if(utils.runScript("docker build -t ${DOCKER_REGISTRY_ENDPOINT}/cdit-ma/re_minimal -f docker/re_minimal/Dockerfile .") != 0) {
                                    error("Failed to build re_minimal image")
                                }
                                if(utils.runScript("docker push ${DOCKER_REGISTRY_ENDPOINT}/cdit-ma/re_minimal") != 0) {
                                    error("Failed to push re_minimal image after build")
                                }

                                if(utils.runScript("docker build -t ${DOCKER_REGISTRY_ENDPOINT}/cdit-ma/re_full -f docker/re_full/Dockerfile ${base_image_arg} .") != 0) {
                                    error("Failed to build re_full image")
                                }
                                if(utils.runScript("docker push ${DOCKER_REGISTRY_ENDPOINT}/cdit-ma/re_full") != 0) {
                                    error("Failed to push re_full image after build")
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
                                if(utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/cdit-ma/re_minimal") != 0){
                                    error("Failed to pull re_minimal docker image on ${node_name}")
                                }

                                if(utils.runScript("docker pull ${DOCKER_REGISTRY_ENDPOINT}/cdit-ma/re_full") != 0){
                                    error("Failed to pull re_full docker image on ${node_name}")
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
