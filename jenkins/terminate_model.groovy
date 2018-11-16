//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: ENV_MANAGER_ENDPOINT
// -String parameter: EXPERIMENT_NAME
// -String parameter: IS_REGEX

@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final env_manager_endpoint = "${ENV_MANAGER_ENDPOINT}"
final experiment_name = "${EXPERIMENT_NAME}"
final is_regex = "${IS_REGEX}" == "true"

stage("Execute Command"){
    if(env_manager_endpoint && experiment_name){
        node("master"){
            def re_path = env.RE_PATH;

            if(re_path){
                command = re_path + "/bin/re_environment_controller -e " + env_manager_endpoint + " -s -n " + experiment_name + (is_regex ? " -r" : "")
                if(utils.runScript(command) != 0){
                    error('Termination of Experiment failed.')
                }
            }else{
                error("RE_PATH not set")
            }
        }
    }else{
        error("Missing Parameters/Environment Variables")
    }
}