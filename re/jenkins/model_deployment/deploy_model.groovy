@Library('jenkins') _
def utils = new jenkins.Utils(this);

def builder_map = [:]
def unstash_map = [:]
def execution_map = [:]

// File to populate with experiment deployment configuration.
// See environment controller for example
final json_file = 'experiment_config.json'

final workspace_dir = env.BUILD_ID
def added_experiment = false

def cleanupExperiment(Boolean remove_experiment){
    if(remove_experiment){
        node('re') {
            script{
                def utils = new jenkins.Utils(this);
                def args = "-s "
                args += "-n \"${params.experiment_name}\" "
                args += "-e ${params.environment_manager_address} "
                if(utils.runReEnvironmentController(args)){
                    echo "Experiment: ${params.experiment_name} Removed"
                }
            }
        }
    }
    //Run Validation on failure
    node("builder"){
        dir("${env.BUILD_ID}/validation"){
            unstash 'model'
            script{
                def utils = new jenkins.Utils(this);
                if(utils.runRegenXSL('generate_validation.xsl', 'model.graphml', 'write_file=true')){
                    archiveArtifacts artifacts: '*.xml'
                }else{
                    error('Validation report generation failed.')
                }
            }
        }
    }
}

pipeline{
    agent none
    parameters{
        string(name: 'experiment_name', defaultValue: '', description: 'The name for the experiment')
        string(name: 'execution_time', defaultValue: '60', description: 'The duration of the experiment')
        string(name: 'environment_manager_address', defaultValue: "${env.ENVIRONMENT_MANAGER_ADDRESS}", description: 'The address of the Environment Manager to use for this experiment')
        string(name: 'log_verbosity', defaultValue: '3', description: 'The logging verbosity (1-10)')
        file(name: 'model', description: 'The model for this experiment')
    }

    stages{
        stage("Add Experiment"){
            steps{
                node("master"){
                    dir(workspace_dir){
                        touch ".dummy"
                        unstashParam 'model', "${workspace_dir}/model.graphml"

                        //Stash and Archive the model file
                        stash includes: 'model.graphml', name: 'model'
                        archiveArtifacts 'model.graphml'
                    }
                }
                node("re"){
                    dir(workspace_dir){
                        script{
                            unstash 'model'
                            def args = "-n \"${params.experiment_name}\" "
                            args += "-e ${params.environment_manager_address} "
                            args += "-a model.graphml"

                            //Add the experiment and pipe the output to the json_file
                            if(utils.runReEnvironmentController(args, "> ${json_file}")){
                                // Archive and stash deployment configuration .json
                                archiveArtifacts json_file
                                stash includes: json_file, name: json_file
                                currentBuild.displayName = "#${env.BUILD_ID} - ${params.experiment_name}"
                                added_experiment = true
                            }else{
                                error("Failed to add experiment ${params.experiment_name} to environment manager.")
                            }
                        }
                    }
                }
            }
        }

        stage("C++ Generation"){
            steps{
                node("builder"){
                    dir("${workspace_dir}/codegen"){
                        unstash 'model'
                        script{
                            if(!utils.runRegenXSL('generate_project.xsl', 'model.graphml')){
                                error('Project code generation failed.')
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
                    dir(workspace_dir){
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
                            //Need to check if each container in node is deployed to docker or native
                            //If node only has containers deployed to docker nodes, no need to build for native
                            // and viceversa
                            def required_oses = []
                            def require_docker = false
                            for(def d in parsed_json['deployments']){
                                def node_require_native = false

                                for(def c in d['containerIds']){
                                    if(c['isDocker']){
                                        require_docker = true
                                    } else {
                                        node_require_native = true
                                    }
                                }

                                if(node_require_native) {
                                    required_oses += utils.getNodeOSVersion(d["id"]["hostName"])
                                }
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

                                                // If building on a raspberry pi, Unix Makefiles and only 2 cores
                                                // This fixes issues caused by ninja oversubscribing the rpi's cpu and ram
                                                if(os.contains("raspi")) {
                                                    if(!utils.buildProject("Unix Makefiles", "-DCMAKE_INSTALL_PREFIX=../install -- -j2", true)){
                                                        error("CMake failed on Builder Node: ${builder_name}")
                                                    }
                                                }
                                                else {
                                                    if(!utils.buildProject("Ninja", "-DCMAKE_INSTALL_PREFIX=../install", true)){
                                                        error("CMake failed on Builder Node: ${builder_name}")
                                                    }
                                                }
                                            }
                                            dir("install"){
                                                dir("lib"){
                                                    //Stash all built libraries
                                                    stash includes: '**', name: stash_name
                                                }
                                            }
                                            deleteDir()
                                        }
                                    }
                                }
                            }

                            if(require_docker){
                                builder_map["docker"] = {
                                    node("docker_builder"){
                                        def stash_name = "code_docker"
                                        dir("${workspace_dir}/${stash_name}"){
                                            print("Building on docker image for docker image")

                                            docker.image("${docker_registry_endpoint}/cdit-ma/re_full").inside("--network host"){
                                                unstash('codegen')
                                                dir("build"){
                                                    utils.runScript(". configure.sh && cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release && cmake --build . --config Release")
                                                }
                                                dir("lib"){
                                                    //Stash all built libraries
                                                    stash includes: '**', name: stash_name
                                                }
                                            }
                                        }
                                    }
                                }
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

        stage("Execute Model"){
            steps{
                //Prepate Execution stages
                node("re"){
                    dir(workspace_dir){
                        script{
                            unstash json_file
                            //Parse the json file
                            def parsed_json = readJSON file: json_file

                            //Construct the execution map
                            for(d in parsed_json['deployments']){
                                def node_name = d["id"]["hostName"]

                                def node_require_native = false
                                def node_require_docker = false
                                for(def c in d['containerIds']){
                                    if(c['isDocker']){
                                        node_require_docker = true
                                    } else {
                                        node_require_native = true
                                    }
                                }

                                //Handle Logan
                                if(d["hasLoganServer"]){
                                    execution_map["LOGAN_${node_name}"] = {
                                        node(node_name){
                                            dir(workspace_dir){
                                                def args = "-n \"${params.experiment_name}\" "
                                                args += "-e ${params.environment_manager_address} "
                                                args += "-a ${IP_ADDRESS}"

                                                if(utils.runScript("${SEM_PATH}/build/bin/logan_managedserver ${args}") != 0){
                                                    error("logan_managedserver failed on Node: ${node_name}")
                                                }

                                                //Archive any sql databases produced
                                                if(findFiles(glob: '**/*.sql').size() > 0){
                                                    archiveArtifacts artifacts: '**/*.sql'
                                                }
                                            }
                                        }
                                    }
                                }

                                unstash_map["RE_${node_name}"] = {
                                    node(node_name){
                                        if(node_require_native){
                                            dir("${WORKSPACE}/../${JOB_NAME}/${workspace_dir}/libs"){
                                                //Unstash the required libraries once for this node, and place in the non-executor specific folder
                                                //Have to run in the lib directory due to dll linker paths
                                                unstash("code_" + utils.getNodeOSVersion(node_name))
                                            }
                                        }
                                        if(node_require_docker) {
                                            dir("${WORKSPACE}/../${JOB_NAME}/${workspace_dir}/docker_libs"){
                                                // Unstash docker compiled libs in seperate dir
                                                unstash("code_docker")
                                            }
                                        }
                                    }
                                }

                                //Handle Containers
                                for(c in d["containerIds"]){
                                    def container_id = c["id"]
                                    def is_docker = c["isDocker"]
                                    def is_master = c["isMaster"]
                                    def map_name = is_master ? "RE_MSTR" : "RE_SLV"
                                    def docker_registry_endpoint = "${env.DOCKER_REGISTRY_ENDPOINT}"

                                    //Is Slave
                                    execution_map["${map_name}_${node_name}_${container_id}"] = {
                                        node(node_name){
                                            //All containers should run out of the same folder unless docker node, then use docker libs dir
                                            def lib_dir = "libs"
                                            if(is_docker) {
                                                lib_dir = "docker_libs"
                                            }
                                            dir("${WORKSPACE}/../${JOB_NAME}/${workspace_dir}/${lib_dir}"){
                                                def args = "-n \"${params.experiment_name}\" "
                                                args += "-e ${params.environment_manager_address} "
                                                args += "-a ${IP_ADDRESS} "
                                                args += "-l . "
                                                args += "-c ${container_id} "

                                                if(is_master){
                                                    args += "-t ${params.execution_time} "
                                                }

                                                if("${params.log_verbosity}"){
                                                    args += "-v ${params.log_verbosity} "
                                                }

                                                if(is_docker) {
                                                    if(docker_registry_endpoint) {
                                                        docker.image("${docker_registry_endpoint}/cdit-ma/re_full").inside("--network host") {
                                                            if(utils.runScript(". configure.sh && /re/bin/re_node_manager ${args}") != 0) {
                                                                error("re_node_manager failed on Node: ${node_name} : ${container_id}")
                                                            }
                                                        }
                                                    } else {
                                                        error("Docker registry endpoint not set")
                                                    }
                                                } else {
                                                    //Run re_node_manager
                                                    if(utils.runScript("${SEM_DIR}/build/bin/re_node_manager ${args}") != 0){
                                                        error("re_node_manager failed on Node: ${node_name}")
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                script{
                    parallel(unstash_map)
                    parallel(execution_map)
                }
            }
        }
    }
    post{
        failure{
            script{
                cleanupExperiment(added_experiment)
            }
        }
        aborted{
            script{
                cleanupExperiment(added_experiment)
            }
        }
    }
}
