#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

final ARCHIVE_NAME = 're.tar.gz'
def builder_map = [:]
pipeline{
    agent none

    stages{
        stage('Checkout'){
            steps{
                node('master'){
                    dir('/srv/git/'){
                        stash includes: ARCHIVE_NAME, name: ARCHIVE_NAME
                    }
                    script{
                        for(def n in nodesByLabel('deploy_re')){
                            def node_name = n
                            if(node_name == ''){
                                node_name = 'master'
                            }

                            builder_map[node_name] = {
                                node(node_name){
                                    dir("${HOME}/re"){
                                        deleteDir()
                                    }
                                    unstash ARCHIVE_NAME
                                    utils.runScript("tar -xf ${ARCHIVE_NAME} -C ${HOME}" )

                                    dir("${HOME}/re/build"){
                                        print("Running in: " + pwd())
                                        def result = utils.buildProject("Ninja", "")
                                        if(!result){
                                            error('Failed to compile')
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        stage('Compile'){
            steps{
                parallel builder_map
            }
        }
    }
}