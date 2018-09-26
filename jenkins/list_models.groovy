//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: ENV_MANAGER_ENDPOINT

@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final env_manager_endpoint = "${ENV_MANAGER_ENDPOINT}"

stage("Execute Command"){
    if(env_manager_endpoint){
        node("master"){
            def re_path = env.RE_PATH;

            if(re_path){
                command = re_path + "/bin/re_environment_controller -e " + env_manager_endpoint + " -l"
                if(utils.runScript(command) != 0){
                    error('List experimentation failed.')
                }
            }else{
                error("RE_PATH not set")
            }
        }
    }else{
        error("Missing Parameters/Environment Variables")
    }
}