/*
Jenkins method library for use in pipeline scripts.

To configure a jenkins server to use this library, see:
 https://jenkins.io/doc/book/pipeline/shared-libraries/
*/

package jenkins

class Utils implements Serializable {
    def context
    Utils(context){
        this.context = context
    }
    //Get list of all jenkins nodes
    @NonCPS
    def nodeNames(){
        return jenkins.model.Jenkins.instance.nodes.collect { node -> node.name }
    }

    def getNodeOSVersion(String node_name){
        def computer = Jenkins.get().getComputer(node_name)
        return computer.getSystemProperties()['os.version']
    }

    def getNodeOSName(String node_name){
        def computer = Jenkins.get().getComputer(node_name)
        return computer.getSystemProperties()['os.name']
    }

    def getLabels(String name){
        def computer = Jenkins.getInstance().getComputer(name)
        def node = computer.getNode()
        if(computer.isOnline()){
            return node.getLabelString()
        }
        return ""
    }

    //Get labelled nodes
    //Returns list of node names with specified label
    def getNodeIpAddress(String node_name){
        def node = jenkins.model.Jenkins.instance.getNode(node_name);
        if(node){
            def ip_addr_list = node.computer.getChannel().call(new hudson.model.Computer.ListPossibleNames());

            for(addr in ip_addr_list){
                if(addr){
                    return addr;
                }
            }
        }
    }

    //Run script, changes to bat if windows detected.
    def runScript(String script, boolean return_status = true){
        if(context.isUnix()){
            if(return_status){
                return context.sh(returnStatus: true, script: script)
            }else{
                return context.sh(returnStdout: true, script: script)
            }
        }
        else{
            if(return_status){
                return context.powershell(returnStatus: true, script: script)
            }else{
                return context.powershell(returnStdout: true, script: script)
            }
        }
    }

    //Get labelled nodes
    //Returns list of node names with specified label
    def getLabelledNodes(String label){
        def filtered_names = []
        def names = nodeNames()
        for(n in nodeNames()){
            if(getLabels(n).contains(label)){
                filtered_names << n
            }
        }
        return filtered_names
    }

    def runRegenXSL(String transform, String file_path, String args=""){
        return runScript("java -jar ${context.env.SEM_DIR}/re/re_gen/saxon.jar -xsl:${context.env.SEM_DIR}/re/re_gen/${transform} -s:${file_path} ${args}") == 0
    }

    def runReEnvironmentController(String args="", String post_args=""){
        return runScript("${context.env.SEM_DIR}/build/bin/re_environment_controller ${args} ${post_args}") == 0
    }

    //Runs a cmake command using the specified generator and options
    def buildProject(String generator, String cmake_options, boolean install=false){
        print "Calling CMake generate"
        def force_remake = false
        if(force_remake){
            //Clear the Cache file
            context.writeFile file:"CMakeCache.txt", text:""
        }
        if(runScript("cmake .. -G \"" + generator + "\" -DCMAKE_BUILD_TYPE=Release " + cmake_options) == 0){
            def build = 'cmake --build . --config Release'
            if(install){
                build += ' --target install'
            }
            print "Calling CMake --build"
            if(runScript(build) == 0){
                return true;
            }
        }
        return false;
    }

    //Trims the extension from a string
    //Fails when file has no extension and but file path contains '.'
    def trimExtension(String filename){
        def index = filename.lastIndexOf(".")
        if (index > 0){
            filename = filename.take(filename.lastIndexOf('.'))
        }
        return filename;
    }
}

