#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

<<<<<<< HEAD
=======
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
>>>>>>> develop
def builder_map = [:]
def execution_map = [:]
final json_file = 'experiment_config.json'

def RunRegenXSL(String transform, String file_path, String args=""){
    def utils = new cditma.Utils(this);
    def re_gen_path = '${RE_PATH}/re_gen/'
    return utils.runScript("java -jar ${re_gen_path}/saxon.jar -xsl:${re_gen_path}/${transform} -s:${file_path} ${args}") == 0
}

def ReEnvironmentController(String args="", String post_args=""){
    def utils = new cditma.Utils(this);
    return utils.runScript("${RE_PATH}/bin/re_environment_controller ${args} ${post_args}") == 0
}

<<<<<<< HEAD
pipeline{
    agent none
    parameters{
        string(name: 'experiment_name', defaultValue: 'hello', description: 'The name for the experiment')
        string(name: 'execution_time', defaultValue: '60', description: 'The duration of the experiment')
        string(name: 'environment_manager_address', defaultValue: "${env.ENVIRONMENT_MANAGER_ADDRESS}", description: 'The address of the Environment Manager to use for this experiment')
        string(name: 'log_verbosity', defaultValue: '3', description: 'The logging verbosity (1-10)')
        file(name: 'model', description: 'The model for this experiment')
=======
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
>>>>>>> develop
    }

    stages{
        stage("Add Experiment"){
            steps{
                node("mitch-pc"){
                    unstashParam 'model', 'model.graphml'
                    //Stash and Archive the model file
                    stash includes: 'model.graphml', name: 'model'
                    archiveArtifacts 'model.graphml'
                
                    script{
                        def args = " -n \"${params.experiment_name}\" -e ${params.environment_manager_address} -a model.graphml"
                        //Add the experiment and pipe the output to the json_file
                        if(ReEnvironmentController(args, "> ${json_file}")){
                            archiveArtifacts json_file
                            stash includes: json_file, name: json_file
                        }else{
                            error("Failed to add experiment ${params.experiment_name} to environment manager.")
                        }
                    }
                }
            }
        }
<<<<<<< HEAD

        stage("C++ Generation"){
            steps{
                node("builder"){
                    dir("${env.BUILD_ID}/codegen"){
                        unstash 'model'
                        script{
                            if(!RunRegenXSL('generate_project.xsl', 'model.graphml')){
                                error('Project code generation failed.')
                            }
                            if(!RunRegenXSL('generate_validation.xsl', 'model.graphml', 'write_file=true')){
                                error('Validation report generation failed.')
                            }
                        }

                        //Construct a zip file with the code
                        zip(zipFile: "test.zip", archive: true)
                        //Stash the generated code
                        stash includes: '**', name: 'codegen'
                    }
                }
            }
        }

        stage("C++ Compilation"){
            steps{
                //Construct the Compilation Map
                node("re"){
                    unstash json_file
                    script{
                        def builder_nodes = [:]
                        
                        //Construct a map of os_version - > node_names
                        for(def node_name in nodesByLabel("builder")){
                            def os_version = utils.getNodeOSVersion(node_name)
                            if(os_version){
                                builder_nodes[os_version] = node_name
                            }
                        }

                        //Parse the json file
                        def parsed_json = readJSON file: json_file
                        
                        //Construct a list of required OS's to compile
                        def required_oses = []
                        for(def d in parsed_json['deployments']){
                            required_oses += utils.getNodeOSVersion(d["id"]["hostName"])
                        }

                        //Compile all required OS's C++ Code
                        for(def required_os in required_oses.unique()){
                            if(!builder_nodes.containsKey(required_os)){
                                error("Cannot find a Builder node with OS: ${required_os}")
                            }
                            //Get the node_name of the builder node for this OS
                            def builder_name = builder_nodes[required_os]

                            builder_map[builder_name] = {
                                node(builder_name){
                                    def os = utils.getNodeOSVersion(builder_name)
                                    def stash_name = "code_${os}"
                                    dir(stash_name){
                                        print("Building on ${builder_name} for OS: ${os}")

                                        unstash('codegen')
                                        dir("build"){
                                            if(!utils.buildProject("Ninja", "")){
                                                error("CMake failed on Builder Node: ${builder_name}")
                                            }
                                        }
                                        dir("lib"){
                                            //Stash all built libraries
                                            stash includes: '**', name: stash_name
                                        }
                                        deleteDir()
                                    }
                                }
=======
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
                        dir("lib"){
                            //Run re_node_manager
                            if(utils.runScript("${RE_PATH}/bin/re_node_manager" + args) != 0){
                                FAILURE_LIST << ("re_node_manager failed on node: " + node_name)
                                FAILED = true
>>>>>>> develop
                            }
                        }
                    }
                }
                //Actually run the compilation
                script{
                    parallel(builder_map)
                }
            }
        }

<<<<<<< HEAD
        stage("Execute Model"){
            steps{
                //Prepate Execution stages
                node("re"){
                    script{
                        unstash json_file
                        //Parse the json file
                        def parsed_json = readJSON file: json_file

                        //Construct the execution map
                        for(d in parsed_json['deployments']){
                            def node_name = d["id"]["hostName"]

                            //Handle Logan
                            if(d["hasLoganServer"]){
                                execution_map["LOGAN_${node_name}"] = {
                                    node(node_name){
                                        def args = " -n \"${params.experiment_name}\" -e ${params.environment_manager_address} -a ${IP_ADDRESS}"
                                        if(utils.runScript("${RE_PATH}/bin/logan_managedserver" + args) != 0){
                                            error("logan_managedserver failed on Node: ${node_name}")
                                        }
                                        
                                        //Archive any sql databases produced
                                        if(findFiles(glob: '**/*.sql').size() > 0){
                                            archiveArtifacts artifacts: '**/*.sql'
                                        }
                                    }
                                }
                            }

                            //Handle Containers
                            for(c in d["containerIds"]){
                                def container_id = c["id"]
                                def is_docker = c["isDocker"]

                                execution_map["RE_${node_name}_${container_id}"] = {
                                    node(node_name){
                                        dir("${container_id}_libs"){
                                            //Unstash the required libraries for this node.
                                            //Have to run in the lib directory due to dll linker paths
                                            def stash_name = "code_" + utils.getNodeOSVersion(node_name)
                                            
                                            def args = " -n \"${params.experiment_name}\""
                                            args += " -e ${params.environment_manager_address}"
                                            args += " -a ${IP_ADDRESS}"
                                            args += " -l ."
                                            args += " -t ${params.execution_time}"
                                            args += " -c ${container_id}"

                                            if("${params.log_verbosity}"){
                                                args += " -v ${params.log_verbosity}"
                                            }
                                            //Run re_node_manager
                                            if(utils.runScript('${RE_PATH}/bin/re_node_manager' + args) != 0){
                                                error("logan_managedserver failed on Node: ${node_name}")
                                            }
                                        }
                                    }
                                }
                            }
=======
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
>>>>>>> develop
                        }
                    }
                }

<<<<<<< HEAD
                script{
                    parallel(execution_map)
=======
                parallel(node_executions)

                //Archive any sql databases produced
                if(findFiles(glob: '**/*.sql').size() > 0){
                    archiveArtifacts artifacts: '**/*.sql'
                }
                //Delete the Dir
                if(CLEANUP){
                    deleteDir()
>>>>>>> develop
                }
            }
        }
    }
<<<<<<< HEAD
    post{
        failure{
            node('re') {
                script{
                    def args = " -n \"${params.experiment_name}\" -e ${params.environment_manager_address} -s"
                    if(ReEnvironmentController(args)){
                        echo "Experiment: ${params.experiment_name} Removed"
                    }
                }
            }
        }
        aborted{
            node('re') {
                script{
                    def args = " -n \"${params.experiment_name}\" -e ${params.environment_manager_address} -s"
                    if(ReEnvironmentController(args)){
                        echo "Experiment: ${params.experiment_name} Removed"
                    }
                }
=======
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
>>>>>>> develop
            }
        }
    }
}