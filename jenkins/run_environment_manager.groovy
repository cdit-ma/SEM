//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: IP_ADDRESS
// -String parameter: PORT
// -String parameter: TAO_NAMING_SERVICE_ENDPOINT
// -String parameter: QPID_BROKER_ENDPOINT
stage("Run Environment Manager"){
    def node_ = env.NODE;

    node(node_){
        def re_path = env.RE_PATH;
        def ip_addr = env.IP_ADDRESS;
        def port = env.PORT;
        def tao_naming_service_endpoint = env.TAO_NAMING_SERVICE_ENDPOINT;
        def qpid_broker_endpoint = env.QPID_BROKER_ENDPOINT;
        def command = "";

        if (re_path && ip_addr && port){
             command = re_path + "/bin/re_environment_manager -a " + ip_addr + " -r " + port
        }else{
            error("Missing Parameters/Environment Variables")
        }

        if(qpid_broker_endpoint){
            command += " -q " + qpid_broker_endpoint
        }
        if(tao_naming_service_endpoint){
            command += " -t " + tao_naming_service_endpoint
        }

        sh command

    }
}
