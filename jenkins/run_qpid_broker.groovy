//This script requires the following Jenkins plugins:

//Requires following parameters in jenkins job:
// -String parameter: IP_ADDRESS
// -String parameter: PORT
stage("Run QPID Broker"){
    def node_ = env.NODE;
    def broker_options = ""

    if(env.BROKER_OPTIONS){
        broker_options = env.BROKER_OPTIONS
    }

    
    node(node_){
        def qpid_root = env.QPID_ROOT;

        if (qpid_root){
            sh qpid_root + "/sbin/qpidd " + broker_options
        }else{
            print("Missing Parameters/Environment Variables")
        }
    }
}
