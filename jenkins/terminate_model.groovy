//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: ENV_MANAGER_ENDPOINT
// -String parameter: EXPERIMENT_NAME
// -String parameter: NODE

@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final node_name = "${NODE}"
final env_manager_endpoint = "${ENV_MANAGER_ENDPOINT}"
final experiment_name = "${EXPERIMENT_NAME}"

stage("Execute Command"){
    if(node_name && env_manager_endpoint && experiment_name){
        node(node_name){
            def re_path = env.RE_PATH;

            if(re_path){
                command = re_path + "/bin/environment_controller -e " + env_manager_endpoint + " -s -n " + experiment_name
                if(utils.runScript(command) != 0){
                    error('Termination of Experiment failed.')
                }
            }else{
                error("RE_PATH not set")
            }
            sh command
        }
    }else{
        error("Missing Parameters/Environment Variables")
    }
}