import jenkins.model.Jenkins;  
import hudson.slaves.SlaveComputer;  
import hudson.slaves.DumbSlave;  
import hudson.plugins.sshslaves.SSHLauncher;
import java.util.regex.*;
import groovy.json.JsonSlurper;

//Get Jenkins Singleton  
jenkins.model.Jenkins jenkins = jenkins.model.Jenkins.getInstance();
ID_COUNTER = 0;

def static Run(nodeName, runCommand)
{
    def output = new java.io.ByteArrayOutputStream();
    def listener = new hudson.util.StreamTaskListener(output);
    def node = hudson.model.Hudson.instance.getNode(nodeName);
    def launcher = node.createLauncher(listener);

    def command = new hudson.util.ArgumentListBuilder();
    command.addTokenized(runCommand);
    def ps = launcher.launch();
    ps = ps.cmds(command).stdout(listener);
    
    def proc = launcher.launch(ps);
    int retcode = proc.join();
    return [retcode, output.toString()]
}


def trimWS(String str){
    return str.trim().replaceAll(" +", " ");
}


def getHost(String name) {
    def computer = jenkins.model.Jenkins.getInstance().getComputer(name);
    def node = computer.getNode();
    def label_string = node.getLabelString();

    def launcher = node.getLauncher();
    if(computer.isOnline() && launcher instanceof SSHLauncher){
        return launcher.getHost();
    }else{
        return "OFFLINE";
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
OUTPUT <<= '<key attr.name="kind" attr.type="string" for="all" id="k1"/>' << nl();
OUTPUT <<= '<key attr.name="type" attr.type="string" for="all" id="k2"/>' << nl();
OUTPUT <<= '<key attr.name="label" attr.type="string" for="all" id="k3"/>' << nl();
OUTPUT <<= '<key attr.name="description" attr.type="string" for="all" id="k4"/>' << nl();
OUTPUT <<= '<key attr.name="url" attr.type="string" for="all" id="k5"/>' << nl();
OUTPUT <<= '<key attr.name="uuid" attr.type="string" for="all" id="k23"/>' << nl();
OUTPUT <<= '<key attr.name="ip_address" attr.type="string" for="all" id="k6"/>' << nl();
OUTPUT <<= '<key attr.name="os" attr.type="string" for="all" id="k7"/>' << nl();
OUTPUT <<= '<key attr.name="architecture" attr.type="string" for="all" id="k8"/>' << nl();
OUTPUT <<= '<key attr.name="os_version" attr.type="string" for="all" id="k9"/>' << nl();
OUTPUT <<= '<key attr.name="readOnly" attr.type="boolean" for="all" id="k11"/>' << nl();
OUTPUT <<= '<key attr.name="index" attr.type="int" for="all" id="k12"/>' << nl();
OUTPUT <<= '<key attr.name="root_path" attr.type="string" for="all" id="k13"/>' << nl();
OUTPUT <<= '<key attr.name="load_time" attr.type="string" for="all" id="k14"/>' << nl();
OUTPUT <<= '<key attr.name="is_online" attr.type="boolean" for="all" id="k15"/>' << nl();
OUTPUT <<= '<key attr.name="user_name" attr.type="string" for="all" id="k16"/>' << nl();
OUTPUT <<= '<key attr.name="version" attr.type="string" for="all" id="version"/>' << nl();
OUTPUT <<= '<key attr.name="isExpanded" attr.type="boolean" for="all" id="k22"/>' << nl();
OUTPUT <<= '<key attr.name="vendor" attr.type="string" for="all" id="vendor"/>' << nl();
OUTPUT <<= '<key attr.name="is_available" attr.type="boolean" for="all" id="is_available"/>' << nl();
OUTPUT <<= '<key attr.name="memory_size" attr.type="int" for="all" id="memory_size"/>' << nl();
OUTPUT <<= '<key attr.name="clock_freq" attr.type="int" for="all" id="clock_freq"/>' << nl();


OUTPUT <<= '<graph edgedefault="directed" id="' << getID() << '">' << nl();
OUTPUT <<= t(1) << '<node id="' << getID() << '">' << nl();
OUTPUT <<= t(2) << '<data key="k1">HardwareDefinitions</data>' << nl();
OUTPUT <<= t(2) << '<graph edgedefault="directed" id="' << getID() << '">' << nl();
OUTPUT <<= t(3) << '<node id="' << getID() << '">' << nl();

//Setup Cluster information
OUTPUT <<= t(3) << '<data key="k1">HardwareCluster</data>' << nl();
OUTPUT <<= t(4) << '<data key="k3">' << SERVER_NAME << '</data>' << nl();
OUTPUT <<= t(4) << '<data key="k5">' << SERVER_URL << '</data>' << nl();
OUTPUT <<= t(6) << '<data key="k23">' << SERVER_URL << '</data>' << nl();
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
    OUTPUT <<= t(6) << '<data key="k6">' << IP << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k5">' << SERVER_URL << c.getUrl() << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k23">' << SERVER_URL << c.getUrl() << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k15">' << online << '</data>' << nl();
    OUTPUT <<= t(6) << '<data key="k11">true</data>' << nl();

    def RE_PATH = c.getEnvironment().get("RE_PATH", "");

    if(!c.isOffline()){
        //Can only get whilst online
        OUTPUT <<= t(6) << '<data key="k7">' << c.getSystemProperties().get("os.name", "") << '</data>' << nl();
        OUTPUT <<= t(6) << '<data key="k8">' << c.getSystemProperties().get("os.arch", "") << '</data>' << nl();
        OUTPUT <<= t(6) << '<data key="k9">' << c.getSystemProperties().get("os.version", "") << '</data>' << nl();
        OUTPUT <<= t(6) << '<data key="k13">' << slave.getRootPath() << '</data>' << nl();
    
        if(RE_PATH != ""){
            try{
                (recode, output) = Run(hostname, RE_PATH + "/bin/list_opencl_devices");
                def flat_output = output.replace("\n", "").replace("\r", "");
                def m = flat_output =~ '(\\{.*\\})'                                           
                assert m instanceof Matcher                                       
                    if (!m) {                                                         
                        throw new RuntimeException("Oops, text not found!")
                }else{
                    def slurper = new JsonSlurper()
                    def result = slurper.parseText(m[0][0])
        
                    OUTPUT <<= t(6) << '<graph edgedefault="directed" id="' << getID() << '">' << nl();
        
                    for(platform in result.platforms){
                        OUTPUT <<= t(7) << '<node id="' << getID() << '">' << nl();
                        OUTPUT <<= t(8) << '<data key="k1">OpenCLPlatform</data>' << nl();  
                        OUTPUT <<= t(8) << '<data key="k3">' << trimWS(platform.name) << '</data>' << nl();  
                        OUTPUT <<= t(8) << '<data key="version">' << platform.version << '</data>' << nl();
                        OUTPUT <<= t(8) << '<data key="vendor">' << platform.vendor << '</data>' << nl();
        
                        OUTPUT <<= t(8) << '<graph edgedefault="directed" id="' << getID() << '">' << nl();
                        for(device in platform.devices){
                            OUTPUT <<= t(9) << '<node id="' << getID() << '">' << nl();
                            OUTPUT <<= t(10) << '<data key="k1">OpenCLDevice</data>' << nl();  
                            OUTPUT <<= t(10) << '<data key="k3">' << trimWS(device.name) << '</data>' << nl();  
                            OUTPUT <<= t(10) << '<data key="is_available">' << device.available << '</data>' << nl();
                            OUTPUT <<= t(10) << '<data key="memory_size">' << device.mem_size << '</data>' << nl();
                            OUTPUT <<= t(10) << '<data key="clock_freq">' << device.max_clock_frequency << '</data>' << nl();
                            OUTPUT <<= t(10) << '<data key="k2">' << device.type << '</data>' << nl();
                            OUTPUT <<= t(10) << '<data key="version">' << device.driver_version << '</data>' << nl();
                            OUTPUT <<= t(9) << '</node>' << nl();
                        }
                        OUTPUT <<= t(9) << '</graph>' << nl();
                        OUTPUT <<= t(8) << '</node>' << nl();
                    }
                    OUTPUT <<= t(7) << '</graph>' << nl();
                }
            }catch(Exception e) {
            
            } 
        }
        OUTPUT <<= t(6) << '<data key="k12">' << sort_order++ << '</data>' << nl();
        OUTPUT <<= t(6) << '<data key="k14">' << LOAD_TIME << '</data>' << nl();
    }
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