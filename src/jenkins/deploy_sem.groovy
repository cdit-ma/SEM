#!groovy

// This isn't actually importing a jenkins library. Find information regarding jenkins shared libraries.
//  See sem/src/jenkins/Utils.groovy
@Library('jenkins') _
import hudson.tasks.test.AbstractTestResultAction
def utils = new jenkins.Utils(this)

final sem_deploy_nodes = nodesByLabel("sem_deploy_node")
final ubuntu_nodes = nodesByLabel("ubuntu18").intersect(sem_deploy_nodes)
final centos_nodes = nodesByLabel("centos7").intersect(sem_deploy_nodes)

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
                        unstashParam 'sem_source_archive', "${env.BUILD_ID}/${sem_source_archive}"
                        if(utils.runScript("tar -xzf ${sem_source_archive}") != 0){
                            error("Cannot extract archive")
                        }
                        // Stash everything in the top level directory found in the tar.
                        //  This takes the form SEM-BRANCH_OR_TAG_NAME, eg.
                        //  SEM-master, SEM-v4.0.0 or SEM-develop
//                         stash name: "sem_source", includes: "**/SEM-*/**"
//                         stash name: "centos_pre_check", includes: "**/SEM-*/re/scripts/centos_7/install_deps.sh"
//                         stash name: "ubuntu_pre_check", includes: "**/SEM-*/re/scripts/ubuntu_18.04/install_deps.sh"
                        stash name: "sem_source", includes: "**"
                        stash name: "centos_pre_check", includes: "**/re/scripts/centos_7/install_deps.sh"
                        stash name: "ubuntu_pre_check", includes: "**/re/scripts/ubuntu_18.04/install_deps.sh"
                    }
                }
            }
        }
        stage("Pre-deploy environment checks"){
            steps{
                script{
                    def pre_build_checks = [:]
                    for(n in centos_nodes){
                        def node_name = n
                        pre_build_checks[node_name] = {
                            node(node_name){
                                dir("${HOME}"){
                                    unstash "centos_pre_check"
                                    utils.runScript("./install_deps.sh")
                                }
                            }
                        }
                    }
                    for(n in ubuntu_nodes){
                        def node_name = n
                        pre_build_checks[node_name] = {
                            node(node_name){
                                dir("${HOME}"){
                                    unstash "ubuntu_pre_check"
                                    utils.runScript("./install_deps.sh")
                                }
                            }
                        }
                    }
                    parallel(pre_build_checks)
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
