//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final KILL_NODE_MANAGER = "${KILL_NODE_MANAGER}" == "true"
final KILL_ENV_MANAGER = "${KILL_ENV_MANAGER}" == "true"
final CLEANUP_WORKSPACE = "${CLEANUP_WORKSPACE}" == "true"

def nodes = nodesByLabel("re")
nodes += nodesByLabel("deploy_re")
def kill_map = [:]

//Construct a builder map
for(n in nodes){
    def node_name = n

    kill_map[node_name] = {
        node(node_name){
            if(CLEANUP_WORKSPACE){
                def workspace_dir = pwd() + "/../"
                print("Removing: " + workspace_dir)
                dir(workspace_dir){
                    deleteDir()
                }
            }

            if(KILL_NODE_MANAGER){
                utils.runScript("pkill -f re_node_manager")
                utils.runScript("pkill -f logan_managedserver")
            }

            if(KILL_ENV_MANAGER){
                utils.runScript("pkill -f re_execution_manager")
            }
        }
    }
}

parallel kill_map