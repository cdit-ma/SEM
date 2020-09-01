#!groovy

// This isn't actually importing a jenkins library. Find information regarding jenkins shared libraries.
//  See sem/src/jenkins/Utils.groovy
@Library('jenkins') _
import hudson.tasks.test.AbstractTestResultAction
def utils = new jenkins.Utils(this)

final sem_deploy_nodes = nodesByLabel("sem_deploy_node")

pipeline{
    agent{node "master"}
    parameters{
        // TODO: Change this to pull from repo once we're open source. Take version tag as param
        file(name: 'sem_source_archive', description: 'The archive to deploy (tar.gz). Should be obtained from the SEM releases page found at https://github.com/cdit-ma/SEM/releases.')
    }
    stages{
        stage("Checkout/Bundle"){
            steps{
                script{
                    final sem_source_archive = "sem_source_archive.tar.gz"

                    dir("${env.BUILD_ID}"){
                        touch(".dummy")
                        unstashParam 'sem_source_archive' "${env.BUILD_ID}/${sem_source_archive}"
                        if(utils.runScript("tar -xf ${sem_source_archive}") != 0){
                            error("Cannot extract archive")
                        }
                        stash name: "sem_source", includes: "**/SEM-*/**"
                    }
                }
            }
        }
        stage("Pre-deploy environment checks"){
            steps{
                script{
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
                                dir("${HOME}/sem"){
                                    deleteDir()
                                    unstash "sem_source"
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
                    }
                    parallel(deploy_map)
                }
            }
        }
    }
}
