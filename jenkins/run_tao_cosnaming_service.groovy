//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: NODE_NAME
// -String parameter: TAO_COSNAMING_PORT
stage("Run TAO cosnaming service"){
    def node_ = env.NODE_NAME;
    def port = env.TAO_COSNAMING_PORT

    node(node_){
        def ip_address = env.IP_ADDRESS;
        def tao_root = env.TAO_ROOT;


        if (tao_root && ip_address && port){
            //TODO: fill the things.
            sh tao_root + "/orbsvcs/Naming_Service/tao_cosnaming -ORBEndpoint iiop://" + ip_address + ":" + port
        }else{
            error("Missing Parameters/Environment Variables")
        }
    }
}
