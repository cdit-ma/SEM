//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: ENV_MANAGER_ENDPOINT
// -String parameter: PORT
// -String parameter: ARGS

stage("Execute Command"){
    def node_ = env.NODE;

    node(node_){
        def re_path = env.RE_PATH;
        def endpoint = env.ENV_MANAGER_ENDPOINT;
        def args = env.ARGS;
        def command = "";

        if (re_path && endpoint && args){
             command = re_path + "/bin/environment_controller -e " + endpoint + " " + args
        }else{
            error("Missing Parameters/Environment Variables")
        }
        sh command
    }
}
