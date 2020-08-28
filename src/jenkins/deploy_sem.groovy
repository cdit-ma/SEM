#!groovy

// This isn't actually importing a jenkins library. Find information regarding jenkins shared libraries.
//  See sem/src/jenkins/Utils.groovy
@Library('jenkins') _
import hudson.tasks.test.AbstractTestResultAction
def utils = new jenkins.Utils(this)

final sem_deploy_nodes = nodesByLabel("sem_deploy_node")

pipeline{
    agent{node "sem_deploy_node"}

    stages{
        stage("Pre-deploy environment checks"){
            steps{
                script{
                }
            }
        }
        stage("Checkout/Bundle"){
            steps{
                script{
                    deleteDir()
                    checkout scm
                    stash includes: "**", name: "source_code"

                    //Read the VERSION.MD
                    if(fileExists("VERSION.md")){
                        RELEASE_DESCRIPTION = readFile("VERSION.md")
                    }
                }
            }
        }

        stage("Compilation"){
            steps{
                script{
                    def deploy_map = [:]
                    for(n in sem_deploy_nodes){
                        def node_name = n
                        builder_map[node_name] = {
                            node(node_name){

                                // TODO: Change into sem install location
                                unstash "source_code"
                                dir("build"){
                                    deleteDir()
                                    def generate_args = "-DBUILD_MEDEA=OFF"
                                    if(env.SEM_DEPS_CACHE_DIR){
                                        generate_args = generate_args + " -DSEM_DEPS_BASE_DIR=${env.SEM_DEPS_CACHE_DIR}"
                                    }
                                    if(!utils.buildProject("Ninja", generate_args)){
                                        error("CMake failed on Builder Node: ${node_name}")
                                    }
                                }
                            }
                        }
                    }
                    parallel(deploy_map)
                }
            }
        }
    }
}
