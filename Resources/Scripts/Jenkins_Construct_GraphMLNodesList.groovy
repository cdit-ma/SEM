//Builds outputs nodelist XML data for all online slave nodes in Jenkins Server
//For use with DeploymentGenerator
int ID_COUNTER = 1;
println('<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns">');
println('<key attr.name="kind" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                       //1
println('<key attr.name="type" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                       //2
println('<key attr.name="label" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                      //3
println('<key attr.name="description" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                //4
println('<key attr.name="url" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                        //5
println('<key attr.name="ip_address" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                 //6
println('<key attr.name="os" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                         //7
println('<key attr.name="architecture" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');               //8
println('<key attr.name="os_version" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');                 //9
println('<key attr.name="shared_directory" attr.type="string" for="node" id="' + (ID_COUNTER++) + '"/>');           //10
println('<key attr.name="readOnly" attr.type="boolean" for="node" id="' + (ID_COUNTER++) + '"/>');                  //11
println('<key attr.name="sortOrder" attr.type="boolean" for="node" id="' + (ID_COUNTER++) + '"/>');                 //12
println('\t<graph edgedefault="directed" id="parentGraph0">');
println('\t<node id="' + (ID_COUNTER++) + '">');
println('\t\t<data key="1">HardwareDefinitions</data>');
println('\t\t<data key="3">HardwareDefinitions</data>');
println('\t\t<graph edgedefault="directed" id="' + ID_COUNTER + 'G">');
println('\t\t\t<node id="' + (ID_COUNTER++) + '">');
println('\t\t\t\t<data key="1">HardwareCluster</data>');

try{
    SERVER_NAME = InetAddress.getLocalHost().getHostName();
    IP = InetAddress.getByName(SERVER_NAME)
}catch(Exception e){
  SERVER_NAME = "UNKNOWN";
  IP = "UNKNOWN";
}

int SORT_ORDER = 0;

println('\t\t\t\t<data key="3">' + SERVER_NAME + ' Jenkins Nodes</data>');
println('\t\t\t\t<data key="4">Hardware Nodes owned by ' + SERVER_NAME + ' @ ' + IP + '</data>');
println('\t\t\t\t<graph edgedefault="directed" id="' + ID_COUNTER + 'G">');
for (slave in hudson.model.Hudson.instance.slaves) {
        if(!slave.getComputer().isOffline()){
                label = slave.getLabelString()
                if(label.contains("CUTS")){
                        println('\t\t\t\t\t<node id="' + (ID_COUNTER++) + '">');
                        println('\t\t\t\t\t\t<data key="1">HardwareNode</data>');
                        println('\t\t\t\t\t\t<data key="3">' + slave.getNodeName() + '</data>');
                        println('\t\t\t\t\t\t<data key="4">' + slave.getLabelString() + '</data>');
                        println('\t\t\t\t\t\t<data key="6">' + slave.getNodeDescription() + '</data>');
                        println('\t\t\t\t\t\t<data key="7">' + slave.getComputer().getSystemProperties().get("os.name") + '</data>');
                        println('\t\t\t\t\t\t<data key="8">' + slave.getComputer().getSystemProperties().get("os.arch") + '</data>');
                        println('\t\t\t\t\t\t<data key="9">' + slave.getComputer().getSystemProperties().get("os.version") + '</data>');
                        println('\t\t\t\t\t\t<data key="10">' + slave.getComputer().getEnvironment().get("sharedir","NOTDEFINED") + '</data>');
                        println('\t\t\t\t\t\t<data key="11">true</data>');
                        println('\t\t\t\t\t\t<data key="12">' + (SORT_ORDER++) +'</data>');
                        println('\t\t\t\t\t</node>');
                }
        }
}
println('\t\t\t\t</graph>');
println('\t\t\t</node>');
println('\t\t</graph>');
println('\t</node>');
println('</graph>');
println('\t</graphml>');
