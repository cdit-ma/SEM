//This script requires the following Jenkins plugins:
//-Pipeline: Utility Steps

//Requires following parameters in jenkins job:
// -String parameter: EXECUTION_TIME
// -String parameter: EXPERIMENT_NAME
// -String parameter: ENVIRONMENT_MANAGER_ADDRESS
//
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final execution_time = "${EXECUTION_TIME}"
final env_manager_addr = "${ENVIRONMENT_MANAGER_ADDRESS}"
final build_id = env.BUILD_ID
final CLEANUP = false

def experiment_name = "${EXPERIMENT_NAME}"

if(experiment_name.isEmpty()){
    experiment_name = "deploy_model_" + build_id
}else{
    currentBuild.displayName = "#" + build_id + " - " + experiment_name
}

//Set the current build description
currentBuild.description = experiment_name

//Executor Maps
def logan_instances = [:]
def builder_map = [:]
def execution_map = [:]

def FAILED = false
def FAILURE_LIST = []
final UNIQUE_ID =  experiment_name + "_" + build_id
final MODEL_FILE = UNIQUE_ID + ".graphml"

def builder_nodes = [:]
def re_nodes = nodesByLabel("re")

def requires_shutdown = false
//Run the environment controller on master
def node_manager_node_names = []
def logan_server_node_names = []


//Get one builder node per architecture
for(node_name in nodesByLabel("builder")){
    def os_version = utils.getNodeOSVersion(node_name)

    if(os_version.length() && !builder_nodes.containsKey(os_version)){
        builder_nodes[os_version] = node_name
    }
}

//Check for builder nodes
if(builder_nodes.size() == 0){
    error('Cannot find any builder nodes.')
}

if(re_nodes.size() == 0){
    error('Cannot find any nodes.')
}


//Generate Project Code on first builder node
node(builder_nodes[0]){
    //Unstash the model from parameter
    unstashParam "model", MODEL_FILE
    sleep(1)
    //Stash the model file
    stash includes: MODEL_FILE, name: 'model'
    
    //Generate the code on the first builder node
    stage('C++ Generation'){
        dir(build_id + "/Codegen"){
            unstash 'model'
            sleep(1)
            
            def re_gen_path = '${RE_PATH}/re_gen/'
            def saxon_call = 'java -jar ' + re_gen_path + '/saxon.jar -xsl:' + re_gen_path
            def file_parameter = ' -s:' + MODEL_FILE
            
            def run_generation = saxon_call + '/generate_project.xsl' + file_parameter

            if(utils.runScript(run_generation) != 0){
                error('Project code generation failed.')
            }

            //Construct a zip file with the code
            zip(zipFile: UNIQUE_ID + ".zip", archive: true)

            //Stash the generated code
            stash includes: '**', name: 'codegen'
            
            //Archive the model
            archiveArtifacts MODEL_FILE
            
            //Delete the Dir
            if(CLEANUP){
                deleteDir()
            }
        }
    }
}


//Register the Experiment
stage("Add Experiment"){
    node("master"){
        dir(UNIQUE_ID){
            unstash "model"
            def controller_path = '${RE_PATH}/bin/re_environment_controller '
            def args = " -e " + env_manager_addr
            args += " -a " + MODEL_FILE
            args += " -n " + experiment_name

            def json_file = "experiment_config.json"
            def command = controller_path + args + ' > ' + json_file

            if(utils.runScript(command) == 0){
                archiveArtifacts json_file
            }else{
                error("Failed to add experiment to environment manager.")
            }
            requires_shutdown = true
            def parsed_json = readJSON file: json_file

            for(def node : parsed_json["nodeManagers"]){
                node_manager_node_names += node["hostName"]
            }
            for(def node : parsed_json["loganServers"]){
                logan_server_node_names += node["hostName"]
            }
        }
    }
}

//Get the required os versions to be compiled
def required_os_versions = []
for(node_name in node_manager_node_names){
    def os_version = utils.getNodeOSVersion(node_name)
    if(!required_os_versions.contains(os_version)){
        required_versions += os_version
    }
}

//Construct the builder map
for(v in required_os_versions){
    def os_version = v
    
    if(builder_nodes[builder_nodes]){
        def node_name = builder_nodes[builder_nodes]
        builder_map[node_name] = {
            node(node_name){
                //Construct a stash name
                def stash_name = "code_" + os_version
                dir(UNIQUE_ID){
                    //Unstash the generated code
                    unstash 'codegen'

                    dir("lib"){
                        //Clear any cached binaries
                        deleteDir()
                    }

                    dir("build"){
                        if(!utils.buildProject("Ninja", "", false)){
                            error("CMake failed on Builder Node: " + node_name)
                        }
                    }

                    dir("lib"){
                        //Stash all built libraries
                        stash includes: '**', name: stash_name
                    }
                    deleteDir()
                }
            }
        }
    }else{
        //Fail if we don't have a 
        FAILURE_LIST += ("Missing Builder Node for OS: '" + os_version + "'")
        FAILED = true
    }
}

//Run compilation scripts
stage("Compiling C++"){
    if(!FAILED){
        parallel builder_map
    }
}

//Produce the execution map
def execution_node_names = (node_manager_node_names + logan_server_node_names).unique(false)
for(n in execution_node_names){
    def node_name = n;
    execution_map[node_name] = {
        node(node_name){
            //Get the ip address
            final ip_addr = env.IP_ADDRESS
            if(!ip_addr){
                error("Runtime Node: " + node_name + " doesn't have an IP_ADDRESS env var.")
            }
            def run_node_manager = node_manager_node_names.contains(node_name)
            def run_logan_server = logan_server_node_names.contains(node_name)

            def node_executions = [:]
            dir(build_id){
                if(run_node_manager){
                    dir("lib"){
                        //Unstash the required libraries for this node.
                        //Have to run in the lib directory due to dll linker paths
                        unstash "code_" + utils.getNodeOSVersion(node_name)
                    
                    }
                    def args = ' -n "' + experiment_name + '"'
                    args += " -e " + env_manager_addr
                    args += " -a " + ip_addr
                    args += " -l ."
                    args += " -t " + execution_time

                    node_executions["RE_" + node_name] = {
                        dir("lib"){
                            //Run re_node_manager
                            if(utils.runScript("${RE_PATH}/bin/re_node_manager" + args) != 0){
                                FAILURE_LIST << ("re_node_manager failed on node: " + node_name)
                                FAILED = true
                            }
                        }
                    }
                }

                if(run_logan_server){
                    def args = ' -n "' + experiment_name + '"'
                    args += " -e " + env_manager_addr
                    args += " -a " + ip_addr

                    node_executions["LOGAN_" + node_name] = {
                        //Run Logan
                        if(utils.runScript("${RE_PATH}/bin/logan_managedserver" + args) != 0){
                            FAILURE_LIST << ("logan_managedserver failed on node: " + node_name)
                            FAILED = true
                        }
                    }
                }
                parallel(node_executions)
                    
                //Archive any sql databases produced
                if(findFiles(glob: '**/*.sql').size() > 0){
                    archiveArtifacts artifacts: '**/*.sql'
                }
                //Delete the Dir
                if(CLEANUP){
                    deleteDir()
                }
            }
        }
    }
}

//Run compilation scripts
stage("Execute Model"){
    parallel execution_map
}

if(FAILED){
    print("Model Execution failed!")
    print(FAILURE_LIST.size() + " Error(s)")
    for(failure in FAILURE_LIST){
        print("ERROR: " + failure)
    }
    if(requires_shutdown){
        stage("Purging Registered Experiment"){
            node("master"){
                def controller_path = '${RE_PATH}/bin/re_environment_controller '
                def args = ' -n ' + experiment_name + ' -e ' + env_manager_addr + ' -s '
                def command = controller_path + args
                utils.runScript(command)
            }
        }
    }
    error("Model Execution failed!")
}