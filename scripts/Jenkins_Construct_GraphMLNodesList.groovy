import jenkins.model.Jenkins;  
import hudson.slaves.SlaveComputer;  
import hudson.slaves.DumbSlave;  
import hudson.plugins.sshslaves.SSHLauncher;  

//Get Jenkins Singleton  
jenkins.model.Jenkins jenkins = jenkins.model.Jenkins.getInstance();
ID_COUNTER = 0;

def getHost(String name) {
    def computer = jenkins.model.Jenkins.getInstance().getComputer(name);
    def node = computer.getNode();
    def label_string = node.getLabelString();

    def launcher = node.getLauncher();
    if(computer.isOnline()){
        return launcher.getHost();
    }else{
        return "";
    }
}

def t(int count=0){
    return "\t" * count;
}

def nl(){
    return "\n";
}

def getID(){
    return String.valueOf(ID_COUNTER ++);
}

SERVER_NAME = ""
SERVER_URL = ""
USER_NAME = ""
LOAD_TIME = (int)(System.currentTimeMillis() / 1000);

try{
    SERVER_URL = jenkins.getRootUrl();
    SERVER_NAME = InetAddress.getLocalHost().getHostName();
    USER_NAME = jenkins.getMe().getDisplayName();
}catch(Exception e){
}
ID_COUNTER = 0

OUTPUT = '<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns">' << nl();
OUTPUT <<= '<key attr.name="kind" attr.type="string" for="node" id="k1"/>' << nl();
OUTPUT <<= '<key attr.name="type" attr.type="string" for="node" id="k2"/>' << nl();
OUTPUT <<= '<key attr.name="label" attr.type="string" for="node" id="k3"/>' << nl();
OUTPUT <<= '<key attr.name="description" attr.type="string" for="node" id="k4"/>' << nl();
OUTPUT <<= '<key attr.name="url" attr.type="string" for="node" id="k5"/>' << nl();
OUTPUT <<= '<key attr.name="ip_address" attr.type="string" for="node" id="k6"/>' << nl();
OUTPUT <<= '<key attr.name="os" attr.type="string" for="node" id="k7"/>' << nl();
OUTPUT <<= '<key attr.name="architecture" attr.type="string" for="node" id="k8"/>' << nl();
OUTPUT <<= '<key attr.name="os_version" attr.type="string" for="node" id="k9"/>' << nl();
OUTPUT <<= '<key attr.name="shared_directory" attr.type="string" for="node" id="k10"/>' << nl();
OUTPUT <<= '<key attr.name="readOnly" attr.type="boolean" for="node" id="k11"/>' << nl();
OUTPUT <<= '<key attr.name="sortOrder" attr.type="int" for="node" id="k12"/>' << nl();
OUTPUT <<= '<key attr.name="root_path" attr.type="string" for="node" id="k13"/>' << nl();
OUTPUT <<= '<key attr.name="load_time" attr.type="string" for="node" id="k14"/>' << nl();
OUTPUT <<= '<key attr.name="is_online" attr.type="boolean" for="node" id="k15"/>' << nl();
OUTPUT <<= '<key attr.name="user_name" attr.type="string" for="node" id="k16"/>' << nl();
OUTPUT <<= '<key attr.name="hostname" attr.type="string" for="node" id="k18"/>' << nl();
OUTPUT <<= '<key attr.name="version" attr.type="string" for="node" id="k17"/>' << nl();
OUTPUT <<= '<key attr.name="isExpanded" attr.type="boolean" for="node" id="k22"/>' << nl();
OUTPUT <<= '<graph edgedefault="directed" id="' << getID() << '">' << nl();
OUTPUT <<= t(1) << '<node id="' << getID() << '">' << nl();
OUTPUT <<= t(2) << '<data key="k1">HardwareDefinitions</data>' << nl();
OUTPUT <<= t(2) << '<graph edgedefault="directed" id="' << getID() << '">' << nl();
OUTPUT <<= t(3) << '<node id="' << getID() << '">' << nl();

//Setup Cluster information
OUTPUT <<= t(3) << '<data key="k1">HardwareCluster</data>' << nl();
OUTPUT <<= t(4) << '<data key="k3">' << SERVER_NAME << '</data>' << nl();
OUTPUT <<= t(4) << '<data key="k5">' << SERVER_URL << '</data>' << nl();
OUTPUT <<= t(4) << '<data key="k16">' << USER_NAME << '</data>' << nl();
OUTPUT <<= t(4) << '<data key="k14">' << LOAD_TIME << '</data>' << nl();
OUTPUT <<= t(4) << '<data key="k22">true</data>' << nl();
OUTPUT <<= t(4) << '<data key="k11">true</data>' << nl();
OUTPUT <<= t(4) << '<graph edgedefault="directed" id="' << getID() << '">' << nl();

sort_order = 0
//Deal with the nodes
for(slave in jenkins.slaves){
    hudson.model.Computer c = slave.getComputer();

    online = c.isOffline() ? "false" : "true";
    hostname = slave.getNodeName();
    IP = getHost(hostname);

    OUTPUT <<= t(5) << '<node id="' << getID() << '">' << nl();
    OUTPUT <<= t(6) << '<data key="k1">HardwareNode</data>' << nl();  
    OUTPUT <<= t(6) << '<data key="k22">true</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k3">' << slave.getNodeName() << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k4">' << slave.getLabelString() << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k18">' << hostname << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k6">' << IP << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k5">' << SERVER_URL << c.getUrl() << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k15">' << online << '</data>' << nl();

	if(!c.isOffline()){
			//Can only get whilst online
			OUTPUT <<= t(6) << '<data key="k7">' << c.getSystemProperties().get("os.name", "") << '</data>' << nl();
			OUTPUT <<= t(6) << '<data key="k8">' << c.getSystemProperties().get("os.arch", "") << '</data>' << nl();
			OUTPUT <<= t(6) << '<data key="k9">' << c.getSystemProperties().get("os.version", "") << '</data>' << nl();
			OUTPUT <<= t(6) << '<data key="k10">' << c.getEnvironment().get("sharedir", "NOTDEFINED") << '</data>' << nl();
			OUTPUT <<= t(6) << '<data key="k13">' << slave.getRootPath() << '</data>' << nl();
	}

    OUTPUT <<= t(6) << '<data key="k12">' << sort_order++ << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k14">' << LOAD_TIME << '</data>' << nl();
    OUTPUT <<= t(5) << '</node>' << nl();
}

//Finish off document
OUTPUT <<= t(4) << '</graph>' << nl();
OUTPUT <<= t(3) << '</node>' << nl();
OUTPUT <<= t(2) << '</graph>' << nl();
OUTPUT <<= t(1) << '</node>' << nl();
OUTPUT <<= '</graph>' << nl();
OUTPUT <<= '</graphml>' << nl();

println(OUTPUT);