//Get Jenkins Singleton
jenkins.model.Jenkins jenkins = jenkins.model.Jenkins.getInstance();

//Setup Defaults
JOB_NAME = "MEDEA-SEM";
SERVER_NAME = "";
SERVER_URL = "";
SERVER_IP = "";
USER_NAME = "";
LOAD_TIME = (int)(System.currentTimeMillis() / 1000);
SLAVES = [];
OFFSET = 100;
INIT_X = 54;
INIT_Y = 140;


if(args.length >= 1){
    //Get Jobname from args
    JOB_NAME = args[0];
}



nodesInJob = [];


//Get Server Specific information.
try{
    SERVER_NAME = InetAddress.getLocalHost().getHostName();
    SERVER_IP = InetAddress.getByName(SERVER_NAME).getHostAddress();
    SERVER_URL = jenkins.getRootUrl();
    USER_NAME = jenkins.getMe().getDisplayName();

    for(job in jenkins.getItems(hudson.matrix.MatrixProject)){
        //Find job which matches input.
        if(job.getDisplayName() == JOB_NAME){
            for(jobname in job.getLabels()){
                nodesInJob += (String) jobname;
            }
        }
    }
}catch(Exception e){}

//SETUP TOP OF GRAPHML
ID_COUNTER = 0
OUTPUT = '<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns">\n';
OUTPUT <<= '<key attr.name="kind" attr.type="string" for="node" id="k1"/>\n';
OUTPUT <<= '<key attr.name="type" attr.type="string" for="node" id="k2"/>\n';
OUTPUT <<= '<key attr.name="label" attr.type="string" for="node" id="k3"/>\n';
OUTPUT <<= '<key attr.name="description" attr.type="string" for="node" id="k4"/>\n';
OUTPUT <<= '<key attr.name="url" attr.type="string" for="node" id="k5"/>\n';
OUTPUT <<= '<key attr.name="ip_address" attr.type="string" for="node" id="k6"/>\n';
OUTPUT <<= '<key attr.name="os" attr.type="string" for="node" id="k7"/>\n';
OUTPUT <<= '<key attr.name="architecture" attr.type="string" for="node" id="k8"/>\n';
OUTPUT <<= '<key attr.name="os_version" attr.type="string" for="node" id="k9"/>\n';
OUTPUT <<= '<key attr.name="shared_directory" attr.type="string" for="node" id="k10"/>\n';
OUTPUT <<= '<key attr.name="readOnly" attr.type="boolean" for="node" id="k11"/>\n';
OUTPUT <<= '<key attr.name="sortOrder" attr.type="string" for="node" id="k12"/>\n';
OUTPUT <<= '<key attr.name="root_path" attr.type="string" for="node" id="k13"/>\n';
OUTPUT <<= '<key attr.name="load_time" attr.type="string" for="node" id="k14"/>\n';
OUTPUT <<= '<key attr.name="is_online" attr.type="boolean" for="node" id="k15"/>\n';
OUTPUT <<= '<key attr.name="user_name" attr.type="string" for="node" id="k16"/>\n';
OUTPUT <<= '<key attr.name="hostname" attr.type="string" for="node" id="k18"/>\n';
OUTPUT <<= '<key attr.name="version" attr.type="string" for="node" id="k17"/>\n';
OUTPUT <<= '<key attr.name="x" attr.type="double" for="node" id="k20"/>\n';
OUTPUT <<= '<key attr.name="y" attr.type="double" for="node" id="k21"/>\n';
OUTPUT <<= '<key attr.name="isExpanded" attr.type="boolean" for="node" id="k22"/>\n';
OUTPUT <<= '\t<graph edgedefault="directed" id="' + (ID_COUNTER++) + '">\n';
OUTPUT <<= '\t<node id="' + (ID_COUNTER++) + '">\n';
OUTPUT <<= '\t\t<data key="k1">HardwareDefinitions</data>\n';
OUTPUT <<= '\t\t<graph edgedefault="directed" id="' + (ID_COUNTER++) + '">\n';
OUTPUT <<= '\t\t\t<node id="' + (ID_COUNTER++) + '">\n';


//Setup Cluster information
OUTPUT <<= '\t\t\t\t<data key="k1">HardwareCluster</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k3">' + SERVER_NAME + '</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k6">' + SERVER_IP + '</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k5">' + SERVER_URL + '</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k16">' + USER_NAME + '</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k14">' + LOAD_TIME + '</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k22">true</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k20">-1</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k21">-1</data>\n';
OUTPUT <<= '\t\t\t\t<data key="k11">true</data>\n';
OUTPUT <<= '\t\t\t\t<graph edgedefault="directed" id="' + (ID_COUNTER++) + '">\n';


//Get the Nodes which are used in the job.
for(slave in jenkins.slaves){
    slaveName = slave.getNodeName()
    if(slaveName in nodesInJob){
        SLAVES += slave
    }
}

//Calculate the MAX which works out the number of nodes in each row/col
MAX = Math.ceil(Math.sqrt(SLAVES.size() + 1))

i = 0
//Deal with the nodes
for(slave in SLAVES){
    hudson.model.Computer c = slave.getComputer();

    sortOrder = i
    x = INIT_X + (((int) (i % MAX)) * OFFSET);
    y = INIT_Y + (((int) (i / MAX)) * OFFSET);
    online = "true";

    if(c.isOffline()){
        online = "false";
    }
    hostname = c.getHostName();
    IP = ""
    try{
        addr = InetAddress.getByName(hostname)
        IP = addr.getHostAddress();
    }catch(Exception e){}


    OUTPUT <<= '\t\t\t\t\t<node id="' + (ID_COUNTER++) + '">\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k1">HardwareNode</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k20">' + x + '</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k21">' + y + '</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k3">' + slave.getNodeName() + '</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k4">' + slave.getLabelString() + '</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k18">' + hostname + '</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k6">' + IP + '</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k5">' + SERVER_URL + "/" + c.getUrl() + '</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k15">' + online + '</data>\n';

        if(!c.isOffline()){
                //Can only get whilst online
                OUTPUT <<= '\t\t\t\t\t\t<data key="k7">' + c.getSystemProperties().get("os.name", "") + '</data>\n';
                OUTPUT <<= '\t\t\t\t\t\t<data key="k8">' + c.getSystemProperties().get("os.arch", "") + '</data>\n';
                OUTPUT <<= '\t\t\t\t\t\t<data key="k9">' + c.getSystemProperties().get("os.version", "") + '</data>\n';
                OUTPUT <<= '\t\t\t\t\t\t<data key="k10">' + c.getEnvironment().get("sharedir", "NOTDEFINED") + '</data>\n';
                OUTPUT <<= '\t\t\t\t\t\t<data key="k13">' + slave.getRootPath() +'</data>\n';
        }

    OUTPUT <<= '\t\t\t\t\t\t<data key="k11">true</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k12">' + sortOrder +'</data>\n';
    OUTPUT <<= '\t\t\t\t\t\t<data key="k14">' + LOAD_TIME +'</data>\n';
    OUTPUT <<= '\t\t\t\t\t</node>\n';
    i++;
}

//Finish off document
OUTPUT <<='\t\t\t\t</graph>\n';
OUTPUT <<='\t\t\t</node>\n';
OUTPUT <<='\t\t</graph>\n';
OUTPUT <<='\t</node>\n';
OUTPUT <<='</graph>\n';
OUTPUT <<='\t</graphml>\n';

println OUTPUT
