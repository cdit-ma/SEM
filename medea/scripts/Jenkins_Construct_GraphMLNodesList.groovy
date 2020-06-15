import jenkins.model.Jenkins;
import hudson.slaves.SlaveComputer;
import hudson.slaves.DumbSlave;
import hudson.plugins.sshslaves.SSHLauncher;
import java.util.regex.*;
import groovy.json.JsonSlurper;

//Get Jenkins Singleton
def jenkins = jenkins.model.Jenkins.getInstance();

ID_COUNTER = 0;

def static Run(String node_name, String str_command){
    def output = new java.io.ByteArrayOutputStream();
    def listener = new hudson.util.StreamTaskListener(output);
    def node = hudson.model.Hudson.instance.getNode(node_name);
    def launcher = node.createLauncher(listener);

    def command = new hudson.util.ArgumentListBuilder();
    command.addTokenized(str_command);
    def ps = launcher.launch();
    ps = ps.cmds(command).stdout(listener);

    def proc = launcher.launch(ps);
    int retcode = proc.join();
    return [retcode, output.toString()]
}

def GetOpenCLDevices(hudson.model.Slave slave){
    def host_name = slave.getNodeName()
    def computer = slave.getComputer();
    def RE_PATH = computer.getEnvironment().get("RE_PATH", "");

    if(RE_PATH){
        try{
            (recode, output) = Run(host_name, RE_PATH + "/bin/list_opencl_devices");
            def flat_output = output.replace("\n", "").replace("\r", "");
            def m = flat_output =~ '(\\{.*\\})'                                           
            assert m instanceof Matcher                                       
            if(m){
                def slurper = new JsonSlurper()
                return slurper.parseText(m[0][0])
            }
        }catch(Exception e){

        }
    }
}

def TrimWS(String str){
    return str.trim().replaceAll(" +", " ");
}

def DoesSlaveHaveLabel(hudson.model.Slave slave, String label){
    for(tag_cloud_entry in slave.getLabelCloud()){
        def l = tag_cloud_entry.item.getDisplayName()
        if(l == label){
            return true;
        }
    }
    return false;
}

def GetHost(String name) {
    def computer = jenkins.model.Jenkins.getInstance().getComputer(name);
    def node = computer.getNode();
    def label_string = node.getLabelString();

    def launcher = node.getLauncher();

    if(computer.isOnline() && launcher instanceof SSHLauncher){
        return launcher.getHost();
    }else if (computer.isOnline() && label_string.contains("cloud")){
        def ssh_connection_string = computer.getEnvironment().get("SSH_CONNECTION", "");
        return ssh_connection_string.split(' ')[2]
    }
    else{
        return "OFFLINE";
    }
}

def t(int count=0){
    return "    " * count;
}

def nl(){
    return "\n";
}

def GetID(){
    return String.valueOf(++ ID_COUNTER);
}

def GetLastID(){
    return String.valueOf(ID_COUNTER);
}

def Key(String key_name, String type, int tab = 0){
    return t(tab) + '<key attr.name="' + key_name + '" attr.type="' + type + '" for="all" id="' + key_name + '"/>' + nl();
}

def Graphml(){
    return '<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns">' + nl();
}

def EndGraphml(){
    return '</graphml>' + nl();
}

def Node(int tab = 0){
    return t(tab) + '<node id="' + GetID() + '">' + nl();
}

def EndNode(int tab = 0){
    return t(tab) + '</node>' + nl();
}

def EndGraph(int tab = 0){
    return t(tab) + '</graph>' + nl();
}

def Graph(int tab = 0, String id = GetLastID()){
    return t(tab) + '<graph edgedefault="directed" id="' + id + ':">' + nl();
}

def Data(String key_id, def value, int tab = 0){
    def str_val = String.valueOf(value)
    if(str_val != ''){
        return t(tab) + '<data key="' + key_id + '">' + str_val + '</data>' + nl();
    }else{
        return t(tab) + '<data key="' + key_id + '" />' + nl();
    }
}

SERVER_NAME = ""
SERVER_URL = ""
USER_NAME = ""
LOAD_TIME = (int)(System.currentTimeMillis() / 1000);

try{
    SERVER_URL = jenkins.getRootUrl();
    SERVER_NAME = InetAddress.getLocalHost().getHostName();
    USER_NAME = jenkins.getMe().getDisplayName();
}catch(Exception e){}

//Export Keys
OUTPUT = Graphml()
OUTPUT += Key("kind", "string");
OUTPUT += Key("type", "string");
OUTPUT += Key("label", "string");
OUTPUT += Key("description", "string");
OUTPUT += Key("url", "string");
OUTPUT += Key("uuid", "string");
OUTPUT += Key("ip_address", "string");
OUTPUT += Key("os", "string");
OUTPUT += Key("architecture", "string");
OUTPUT += Key("os_version", "string");
OUTPUT += Key("readOnly", "boolean");
OUTPUT += Key("index", "int");
OUTPUT += Key("root_path", "string");
OUTPUT += Key("load_time", "string");
OUTPUT += Key("is_online", "boolean");
OUTPUT += Key("user_name", "string");
OUTPUT += Key("version", "string");
OUTPUT += Key("isExpanded", "boolean");
OUTPUT += Key("is_available", "boolean");
OUTPUT += Key("vendor", "string");
OUTPUT += Key("memory_size", "long");
OUTPUT += Key("clock_freq", "long");

OUTPUT += Graph();
OUTPUT += Node(1);
OUTPUT += Data('kind', 'HardwareDefinitions', 2);
OUTPUT += Graph(2);
OUTPUT += Node(3);
OUTPUT += Data('kind', 'HardwareCluster', 4);
OUTPUT += Data('label', SERVER_NAME, 4);
OUTPUT += Data('url', SERVER_URL, 4);
OUTPUT += Data('uuid', SERVER_URL, 4);
OUTPUT += Data('user_name', USER_NAME, 4);
OUTPUT += Data('load_time', LOAD_TIME, 4);
OUTPUT += Data('isExpanded', true, 4);
OUTPUT += Data('readOnly', true, 4);
OUTPUT += Data('is_online', true, 4);
OUTPUT += Graph(4);

def index = 0
for(slave in jenkins.slaves){
    //Ignore non-re slaves
    if(!DoesSlaveHaveLabel(slave, "re")){
        continue;
    }

    def host_name = slave.getNodeName()
    def computer = slave.getComputer();
    def system_properties = computer.getSystemProperties();
    def is_online = !computer.isOffline()
    def node_uuid = SERVER_URL + computer.getUrl()

    OUTPUT += Node(5);
    OUTPUT += Data('kind', 'HardwareNode', 6);
    OUTPUT += Data('label', host_name, 6);
    OUTPUT += Data('isExpanded', true, 6);
    OUTPUT += Data('description', slave.getLabelString(), 6);
    OUTPUT += Data('ip_address', GetHost(host_name), 6);
    OUTPUT += Data('url', SERVER_URL + computer.getUrl(), 6);
    OUTPUT += Data('uuid', node_uuid, 6);
    OUTPUT += Data('is_online', is_online, 6);
    OUTPUT += Data('readOnly', true, 6);
    OUTPUT += Data('root_path', slave.getRootPath(), 6);
    OUTPUT += Data('index', index++, 6);
    OUTPUT += Data('load_time', LOAD_TIME, 6);

    if(is_online){
        OUTPUT += Data('os', system_properties.get("os.name"), 6);
        OUTPUT += Data('architecture', system_properties.get("os.arch"), 6);
        OUTPUT += Data('os_version', system_properties.get("os.version"), 6);

        def opencl_json = GetOpenCLDevices(slave)
        if(opencl_json){
            OUTPUT += Graph(6);

            for(platform in opencl_json.platforms){
                def platform_name = TrimWS(platform.name)
                def platform_uuid = node_uuid + "/" + platform_name
                OUTPUT += Node(7);
                OUTPUT += Data('kind', 'OpenCLPlatform', 8);
                OUTPUT += Data('label', platform_name, 8);
                OUTPUT += Data('version', platform.version, 8);
                OUTPUT += Data('vendor', platform.vendor, 8);
                OUTPUT += Data('uuid', platform_uuid, 8);
                OUTPUT += Graph(8);

                for(device in platform.devices){
                    def device_name = TrimWS(device.name)
                    def device_uuid = platform_uuid + "/" + device_name
                    OUTPUT += Node(9);
                    OUTPUT += Data('kind', 'OpenCLDevice', 10);
                    OUTPUT += Data('label', TrimWS(device.name), 10);
                    OUTPUT += Data('is_available', device.available, 10);
                    OUTPUT += Data('memory_size', device.mem_size, 10);
                    OUTPUT += Data('clock_freq', device.max_clock_frequency, 10);
                    OUTPUT += Data('type', device.type, 10);
                    OUTPUT += Data('uuid', device_uuid, 10);
                    OUTPUT += Data('vendor', device.driver_version, 10);
                    OUTPUT += EndNode(9);
                }

                OUTPUT += EndGraph(8);
                OUTPUT += EndNode(7);
            }
            OUTPUT += EndGraph(6);
        }
    }
    OUTPUT += EndNode(5);
}

OUTPUT += EndGraph(4);
OUTPUT += EndNode(3);
OUTPUT += EndGraph(2);
OUTPUT += EndNode(1);
OUTPUT += EndGraph();
OUTPUT += EndGraphml();
println(OUTPUT);