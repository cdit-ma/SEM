//This script requires the following Jenkins plugins:
//-Pipeline: Utility Steps

//Requires following parameters in jenkins job:
// -String parameter: EXECUTION_TIME
// -String parameter: EXPERIMENT_NAME
// -String parameter: ENVIRONMENT_MANAGER_ADDRESS
// -String parameter: LOG_VERBOSITY
//
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final execution_time = "${EXECUTION_TIME}"
final env_manager_addr = "${ENVIRONMENT_MANAGER_ADDRESS}"
def log_verbosity = "";
try{
    log_verbosity = "${LOG_VERBOSITY}"
}catch(Exception ex){
}

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

def builder_nodes = []
def nodes = nodesByLabel("re")

def docker_image_repository = "192.168.111.98:5000"
def docker_image_name = "re_full"

def docker_nodes = nodesByLabel("docker_runtime")

//Get the builder nodes
for(node_name in nodesByLabel("builder")){
    builder_nodes += node_name
}

if(docker_nodes.size() != 0){
    builder_nodes += docker_nodes[0]
}

if(builder_nodes.size() == 0){
    error('Cannot find any builder nodes.')
}

if(nodes.size() == 0){
    error('Cannot find any nodes.')
}

//Run codegen on the first node
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
            def file_parameter = ' -s:"' + MODEL_FILE + '"'
            
            def run_generation = saxon_call + '/generate_project.xsl' + file_parameter
            def run_validation = saxon_call + '/generate_validation.xsl' + file_parameter + ' write_file=true'

            if(utils.runScript(run_generation) != 0){
                error('Project code generation failed.')
            }

            if(utils.runScript(run_validation) != 0){
                error('Validation report generation failed.')
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

//Run Compilation
for(n in builder_nodes){
    def node_name = n;
    //Define the Builder instructions
    builder_map[node_name] = {
        if(docker_nodes.contains(node_name)){
            node(node_name){
                //Cache dir is the experiment_name
                docker.image(docker_image_repository + "/" + docker_image_name).inside() {
                    def stash_name = "code_" + utils.getNodeOSVersion(node_name)
                    print(stash_name)
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
        } else {
            node(node_name){
                //Cache dir is the experiment_name
                def stash_name = "code_" + utils.getNodeOSVersion(node_name)
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
    }
}

//Run compilation scripts
stage("Compiling C++"){
    parallel builder_map
}

def requires_shutdown = false

//Run the environment controller on master
def node_containers = [:]
def logan_server_node_names = []
def container_docker_flags = [:]

stage("Add Experiment"){
    node("master"){
        dir(UNIQUE_ID){

            unstash "model"
            def controller_path = '${RE_PATH}/bin/re_environment_controller '
            def args = ' -n "' + experiment_name + '" -e ' + env_manager_addr + ' -a ' + MODEL_FILE
            def json_file = "experiment_config.json"
            def command = controller_path + args + ' > ' + json_file

            if(utils.runScript(command) == 0){
                archiveArtifacts json_file
            }else{
                error("Failed to add experiment to environment manager.")
            }
            requires_shutdown = true
            def parsed_json = readJSON file: json_file

            def deployment_message = "Running re_node_manager on:\n"
            for(def deployment : parsed_json["deployments"]){
                def deployment_host_name = deployment["id"]["hostName"]
                deployment_message += "* " + deployment_host_name + "\n"
                def container_id_list = []

                for(def container_id : deployment["containerIds"]){
                    deployment_message += "** " + container_id["id"] + "\n"
                    container_id_list += container_id["id"]
                    container_docker_flags[container_id["id"]] = container_id["isDocker"]
                    print(container_id["isDocker"])
                    print(container_docker_flags[container_id["id"]])
                }
                node_containers[deployment_host_name] = container_id_list

                if(deployment["hasLoganServer"]){
                    deployment_message += "** LoganServer" + "\n"
                    logan_server_node_names += deployment_host_name
                }
            }
            print(deployment_message)
        }
    }
}

def node_manager_node_names = node_containers.keySet()

def execution_node_names = (node_manager_node_names + logan_server_node_names).unique(false)
//Produce the execution map
for(n in execution_node_names){
    def node_name = n;
    execution_map[node_name] = {
        node(node_name){
            //Get the ip address
            final ip_addr = env.IP_ADDRESS
            if(!ip_addr){
                error("Runtime Node: " + node_name + " doesn't have an IP_ADDRESS env var.")
            }

            dir(build_id){
                def container_ids = node_containers[node_name]
                def run_node_manager = node_manager_node_names.contains(node_name)
                def run_logan_server = logan_server_node_names.contains(node_name)

                def node_executions = [:]

                if(run_node_manager){
                    dir("lib"){
                        //Unstash the required libraries for this node.
                        //Have to run in the lib directory due to dll linker paths
                        unstash "code_" + utils.getNodeOSVersion(node_name)
                    }
                }

                for(def container_id : container_ids){
                    def is_docker = container_docker_flags[container_id]
                    def args = ' -n "' + experiment_name + '"'
                    args += " -e " + env_manager_addr
                    args += " -a " + ip_addr
                    args += " -l ."
                    args += " -t " + execution_time
                    args += " -c " + container_id
                    if(log_verbosity){
                        args += " -v " + log_verbosity
                    }

                    // Add execution for each container running on this node
                    node_executions["RE_" + node_name + "_" + container_id] = {
                        if(is_docker){
                            docker.image(docker_image_repository + "/" + docker_image_name).inside("--network host") {
                                dir("lib"){
                                    //Run re_node_manager
                                    if(utils.runScript("/re/bin/re_node_manager" + args) != 0){
                                        FAILURE_LIST << ("re_node_manager failed on node: " + node_name)
                                        FAILED = true
                                    }
                                }
                            }
                        } else {
                            dir("lib"){
                                //Run re_node_manager
                                if(utils.runScript("${RE_PATH}/bin/re_node_manager" + args) != 0){
                                    FAILURE_LIST << ("re_node_manager failed on node: " + node_name)
                                    FAILED = true
                                }
                            }
                        }
                    }
                }

                // Run a logan server on the node if specified.
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
                def args = ' -n "' + experiment_name + '" -e ' + env_manager_addr + ' -s '
                def command = controller_path + args
                utils.runScript(command)
            }
        }
    }
    error("Model Execution failed!")
}