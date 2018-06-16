//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: IP_ADDRESS
// -String parameter: PORT
stage("Run Environment Manager"){
    def node_ = env.NODE;

    node(node_){
        def re_path = env.RE_PATH;
        def ip_addr = env.IP_ADDRESS;
        def port = env.PORT;

        if (re_path && ip_addr && port){
            sh re_path + "/bin/environment_manager -a " + ip_addr + " -r " + port
        }else{
            print("Missing Parameters/Environment Variables")
        }
    }
}
