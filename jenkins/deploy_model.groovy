//This script requires the following Jenkins plugins:
//-Pipeline: Utility Steps

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
import hudson.model.Computer.ListPossibleNames

def utils = new Utils(this);

def runScriptPid(String script){
    def out = ""
    if(isUnix()){
        def command = script + " > /dev/null 2>&1 & echo \$!"
        withEnv(['JENKINS_NODE_COOKIE=do_not_kill']){
            out = sh(returnStdout: true, script: command)
        }
        return out.trim()
    }
    else{
        //TODO: do windows things here
    }
}

def blockingKill(String pid){
    if(isUnix()){
        def out = sh(returnStatus: true, script:"kill " + pid)
        for(def i = 0; i<5; i++){
            out = sh(returnStatus: true, script: "kill " + pid)
            if(out != 0){
                break;
            }
            sleep(1)
        }
        if(out == 0){
            def out2 = sh(returnStatus: true, script:"kill -s 0 " + pid)
            print(out2)
        }
    }
    else{
        //TODO: do windows things here
    }
}


def masterNode = "${MASTER_NODE}"
def executionTime = "${EXECUTION_TIME}"
def experimentID = env.BUILD_ID
def buildDir = "run" + experimentID


//Deployment plans
def loganServers = [:]
def loganClients = [:]
def loganServers_shutdown = [:]
def loganClients_shutdown = [:]
def experimentMasters = [:]
def experimentSlaves = [:]
def master_shutdown = [:]
def compileCode = [:]
def addrMap = [:]

def fail_flag = false
def failureList = []

def jDeployment = "";

def file = "model.graphml"

def reNodes = utils.getLabelledNodes("envmanager_test")


//XXX: Problems ahoy here
//In case where node has multiple network interfaces, we're cooked.
for(def i = 0; i < reNodes.size(); i++){
    def nodeName = reNodes[i];
    def nodeIPLookup = jenkins.model.Jenkins.instance.getNode(nodeName)
    def ip_addr_list = nodeIPLookup.computer.getChannel().call(new ListPossibleNames())
    addrMap[nodeName] = ip_addr_list[0]
}

def environmentManagerIp = "192.168.224.100"
def environmentManangerPort = "20000"

withEnv(["model=''"]){
    node(masterNode){

        def workspacePath = pwd()
        def reGenPath = "${RE_GEN_PATH}"
        def saxonPath = reGenPath

        def middlewareString = ' middlewares='
        def fileString = ' -s:' + workspacePath + "/" + file
        def jarString = 'java -jar '  + saxonPath + '/saxon.jar -xsl:' + reGenPath
        
        def buildPath = workspacePath + "/" + buildDir
        unstashParam "model", file
        stash includes: file, name: 'model'

        //TODO: Fix this to actually get middlewares from somewhere
        middlewareString += "zmq,proto"
        //Generate C++ code
        dir(buildPath){
            unstash 'model'
            archiveArtifacts file
            
            stage('C++ Generation'){
                def typeGenCommand = jarString + '/g2datatypes.xsl' + fileString + middlewareString
                if(utils.runScript(typeGenCommand) != 0){
                    failureList << "Datatype code generation failed"
                    fail_flag = true;
                } 
                def componentGenCommand = jarString + '/g2components.xsl' + fileString + middlewareString
                if(utils.runScript(componentGenCommand) !=0){
                    failureList << "Component code generation failed"
                    fail_flag = true;
                }

                dir("lib"){
                    //Generate QOS into lib directory
                    def qosGenCommand = jarString + '/g2qos.xsl' + fileString + middlewareString
                    print(qosGenCommand)
                    if(utils.runScript(qosGenCommand) != 0){
                        failureList << "QoS generation failed"
                        fail_flag = true;
                    }
                }
            }
            stage('Archive'){
                stash includes: '**', name: 'codeGen'
            }
        }
    }

    
    stage("Build deployment plan"){
    for(def i = 0; i < reNodes.size(); i++){
        def nodeName = reNodes[i];
        def ipAddr = addrMap[nodeName]

        //Update the map to include the compile
        compileCode[nodeName] = {
            node(nodeName){
                dir(buildDir){
                    unstash 'codeGen'
                }
                dir(buildDir + "/build"){
                    if(!utils.buildProject("Ninja", "")){
                        failureList << ("cmake failed on node: " + nodeName)
                        fail_flag = true;
                    }
                }
            }
        }

        experimentMasters[nodeName] = {
            node(nodeName){
                if(nodeName == masterNode){
                    def workspacePath = pwd()
                    def buildPath = workspacePath + "/" + buildDir + "/lib"
                    def master_command = ""
                        
                        def master_args = " -m " + ipAddr
                        master_args += " -d " + file
                        master_args += " -t " + executionTime
                        master_args += " -n " + experimentID
                        master_args += " -e tcp://" + environmentManagerIp + ":" + environmentManangerPort

                        master_command = "${RE_PATH}" + "/bin/re_experiment_master" + master_args


                    dir(buildPath){
                        unstash 'model'
                        def out
                        out = runScriptPid(master_command)
                        master_shutdown[nodeName] = {
                            node(nodeName){
                                blockingKill(out)
                            }
                        }
                    }
                }
            }
        }

        experimentSlaves[nodeName] = {
            node(nodeName){
                def workspacePath = pwd()
                def buildPath = workspacePath + "/" + buildDir + "/lib"
                def slave_command = ""

                def slave_args = " -s " + ipAddr
                slave_args += " -l . "
                slave_args += " -n " + experimentID
                slave_args += " -e tcp://" + environmentManagerIp + ":" + environmentManangerPort
                slave_command = "${RE_PATH}" + "/bin/re_experiment_slave" + slave_args

                dir(buildPath){
                    if(utils.runScript(slave_command) != 0){
                        failureList << ("Experiment slave failed on node: " + nodeName)
                        fail_flag = true
                    }
                }
            }
        }

    }
    }

    stage("Compiling C++"){
        parallel compileCode
    }

    stage("Execute Model"){
        if(!fail_flag){
            parallel experimentMasters
            sleep 3
            parallel experimentSlaves
        }
        else{
            print("############# Execution skipped, compilation or code generation failed! #############")
        }
    }

    stage("Kill masters"){
        parallel master_shutdown
    }

    if(fail_flag){
        currentBuild.result = 'FAILURE'
        print("Execution failed!")
        print(failureList.size() + " Error(s)")
        for(s in failureList){
            print("ERROR: " + s)
        }
    }
    else{
        currentBuild.result = 'SUCCESS'
    }
}
