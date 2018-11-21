#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

pipeline{
    agent{node "re"}
    parameters{
        booleanParam(name: 'kill_node_manager', defaultValue: true, description: 'Teardown any running re_node_managers/logan_managedservers.')
        booleanParam(name: 'kill_env_manager', defaultValue: true, description: 'Teardown any running re_environment_managers.')
        booleanParam(name: 'cleanup_workspace', defaultValue: true, description: 'Remove all cached files from the jenkins workspace.')
    }

    stages{
        stage("Cleanup Environment"){
            steps{
                script{
                    def kill_map = [:]
                    
                    def nodes = nodesByLabel("re") + nodesByLabel("deploy_re")

                    for(n in nodes.unique()){
                        def node_name = n
                        if(node_name == ""){
                            node_name = "master"
                        }
                        kill_map[node_name] = {
                            node(node_name){
                                if(params.cleanup_workspace){
                                    def workspace_dir = pwd() + "/../"
                                    print("Removing: ${workspace_dir}")
                                    dir(workspace_dir){
                                        deleteDir()
                                    }
                                }

                                if(params.kill_node_manager){
                                    utils.runScript("pkill -9 re_node_manager")
                                    utils.runScript("pkill -9 logan_managedserver")
                                }

                                if(params.kill_env_manager){
                                    utils.runScript("pkill -9 re_environment_manager")
                                }
                            }
                        }
                    }
                    parallel(kill_map)
                }
            }
        }
    }
}