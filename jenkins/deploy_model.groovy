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
def buildArchiveDir = "build" + experimentID

//Deployment plans
def loganServers = [:]
def loganClients = [:]
def loganServers_shutdown = [:]
def loganClients_shutdown = [:]
def experimentMasters = [:]
def experimentSlaves = [:]
def compileCode = [:]
def addrMap = [:]

def fail_flag = false
def failureList = []

def jDeployment = "";

def file = "model.graphml"

def reNodes = utils.getLabelledNodes("envmanager_test")

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
        deleteDir()
        unstashParam "model", file
        stash includes: file, name: 'model'
        archiveArtifacts file
        def workspacePath = pwd()
        def reGenPath = "${RE_GEN_PATH}"
        def saxonPath = reGenPath

        def middlewareString = ' middlewares='
        def fileString = ' -s:' + workspacePath + "/" + file
        def jarString = 'java -jar '  + saxonPath + '/saxon.jar -xsl:' + reGenPath
        //Parse graphml model and generate deployment plan
        stage('Build Deployment Plan'){
            //Generate deployment json
            // def executionParser = "${RE_PATH}" + '/bin/re_execution_parser ' + file + ' > execution.json'
            // sh(script: executionParser)
            // archiveArtifacts "execution.json"

            // def execution_debug = "${RE_PATH}" + '/bin/re_execution_tester -d ' + file + ' > execution.dump'
            // sh(script: execution_debug)
            // archiveArtifacts "execution.dump"

            //Parse json
            // jDeployment = readJSON file: 'execution.json'
        
        }

        // def middlewares = jDeployment["model"]["middlewares"]

        // for(def i = 0; i < middlewares.size(); i++){
        //     middlewareString += middlewares[i]
        //     if(i != middlewares.size()-1){
        //         middlewareString += ","
        //     }
        // }
        
        def buildPath = workspacePath + "/" + buildDir
        middlewareString += "zmq,proto"
        //Generate C++ code
        dir(buildPath){
            unstash "model"
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
        }

        stage('Archive'){
            //Store an archive of generated C++ files
            zip(zipFile: "archive.zip", archive: true, dir: buildPath)
            dir(workspacePath){
                // Stash generated cpp files.
                stash includes: buildDir + '/**', name: 'codeGen'
            }
        }
    }

    // def modelName = jDeployment["model"].name
    // def modelDescription = jDeployment["model"].description
    // currentBuild.description = modelName
    // if(modelDescription){
    //     currentBuild.description = currentBuild.description + " : " + modelDescription
    // }
    
    stage("Build deployment plan"){
    for(def i = 0; i < reNodes.size(); i++){
        def nodeName = reNodes[i];
        def ipAddr = addrMap[nodeName]

        //Update the map to include the compile
        compileCode[nodeName] = {
            node(nodeName){
                unstash 'codeGen'
                dir(buildDir + "/build"){
                    if(!utils.buildProject("Ninja", "")){
                        failureList << ("cmake failed on node: " + nodeName)
                        fail_flag = true;
                    }
                }
            }
        }

        //if we've found the node we plan on running master on, build master instructions
        experimentMasters[nodeName] = {
            if(nodeName == masterNode){
                node(nodeName){
                    def workspacePath = pwd()
                    def buildPath = workspacePath + "/" + buildDir + "/lib"

                    def args = " -m " + ipAddr
                    args += " -d " + file
                    args += " -t " + executionTime
                    args += " -n " + experimentID
                    args += " -e tcp://" + environmentManagerIp + ":" + environmentManangerPort

                    dir(buildPath){
                        unstash 'model'
                        def command = "${RE_PATH}" + "/bin/re_experiment_master" + args
                        if(utils.runScript(command) != 0){
                            failureList << ("Experiment master failed on node: " + nodeName)
                            fail_flag = true
                        }
                    }
                }
            }
            else{
                node(nodeName){
                    def workspacePath = pwd()
                    def buildPath = workspacePath + "/" + buildDir + "/lib"

                    def args = " -s " + ipAddr
                    args += " -l . "
                    args += " -n " + experimentID
                    args += " -e tcp://" + environmentManagerIp + ":" + environmentManangerPort

                    dir(buildPath){
                        unstash 'model'
                        def command = "sleep 3 && " + "${RE_PATH}" + "/bin/re_experiment_slave" + args
                        if(utils.runScript(command) != 0){
                            failureList << ("Experiment slave failed on node: " + nodeName)
                            fail_flag = true
                        }
                    }

                }
            }
        }

        /*
        if(jNode["logan_server"]){
            def args = ""
            def databaseFile = ""

            for(arg in jNode["logan_server"]){
                if(arg.key == "database"){
                    databaseFile = arg.value
                }
                args +=" --" + arg.key 
                if(arg.value instanceof Collection){
                    for(v in arg.value){
                        args += " " + v
                    }
                }else{
                    args += " " + arg.value;
                }
            }

            //Update the map to include the launch instructions for logan_server
            loganServers[nodeName] = {
                node(nodeName){
                    def out
                    dir(buildDir){
                        def serverRun = "${LOGAN_PATH}" + "/bin/logan_server" + args;
                        out = runScriptPid(serverRun)
                    }
                    loganServers_shutdown[nodeName] = {
                        node(nodeName){
                            dir(buildDir){
                                blockingKill(out)
                                archiveArtifacts databaseFile
                            }
                        }
                    }
                }
            }
        }

        */

        /*
        if(jNode["logan_client"]){
            def args = "";

            for(arg in jNode["logan_client"]){
                args +=  " --" + arg.key + " " + arg.value;
            }

            //Update the map to include the launch instructions for logan_client
            loganClients[nodeName] = {
                node(nodeName){
                    def clientRun = "${LOGAN_PATH}" + "/bin/logan_client" + args;
                    def out = runScriptPid(clientRun)
                    loganClients_shutdown[nodeName] = {
                        node(nodeName){
                            blockingKill(out)
                        }
                    }
                }
            }
        }
        */
    }
    }

    // stage("Start logan servers"){
    //     parallel loganServers
    // }

    // stage("Start logan clients"){
    //     parallel loganClients
    // }

    stage("Compiling C++"){
        parallel compileCode
    }

    stage("Execute Model"){
        if(!fail_flag){
            parallel experimentMasters
        }
        else{
            print("############# Execution skipped, compilation or code generation failed! #############")
        }
    }

    // stage("Stop logan clients"){
    //     parallel loganClients_shutdown
    // }
    // stage("Stop logan servers"){
    //     parallel loganServers_shutdown
    // }

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


