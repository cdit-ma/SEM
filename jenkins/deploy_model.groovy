#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);

def builder_map = [:]
def unstash_map = [:]
def execution_map = [:]

final json_file = 'experiment_config.json'

final workspace_dir = env.BUILD_ID
def added_experiment = false

def cleanupExperiment(Boolean remove_experiment){
    if(remove_experiment){
        node('re') {
            script{
                def utils = new cditma.Utils(this);
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
                def utils = new cditma.Utils(this);
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
        string(name: 'docker_registry_address', defaultValue: '${env.DOCKER_REGISTRY_ADDRESS}', description: 'The address of the docker registry.')
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

                                //Handle Logan
                                if(d["hasLoganServer"]){
                                    execution_map["LOGAN_${node_name}"] = {
                                        node(node_name){
                                            dir(workspace_dir){
                                                def args = "-n \"${params.experiment_name}\" "
                                                args += "-e ${params.environment_manager_address} "
                                                args += "-a ${IP_ADDRESS}"

                                                if(utils.runScript("${RE_PATH}/bin/logan_managedserver ${args}") != 0){
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
                                        dir("${WORKSPACE}/../${JOB_NAME}/${workspace_dir}/libs"){
                                            //Unstash the required libraries once for this node, and place in the non-executor specific folder
                                            //Have to run in the lib directory due to dll linker paths
                                            unstash("code_" + utils.getNodeOSVersion(node_name))
                                        }
                                    }
                                }

                                //Handle Containers
                                for(c in d["containerIds"]){
                                    def container_id = c["id"]
                                    def is_docker = c["isDocker"]
                                    def is_master = c["isMaster"]
                                    def map_name = is_master ? "RE_MSTR" : "RE_SLV"

                                    //Is Slave
                                    execution_map["${map_name}_${node_name}_${container_id}"] = {
                                        node(node_name){
                                            //All containers should run out of the same folder
                                            dir("${WORKSPACE}/../${JOB_NAME}/${workspace_dir}/libs"){
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
                                                    docker.image("${docker_registry_address}:5000/re_full").inside("--network host") {
                                                        if(utils.runScript("export NDDSHOME=/opt/RTI/rti_connext_dds-5.3.0 && . /opt/HDE/x86_64.linux/release.com && bash /opt/RTI/rti_connext_dds-5.3.0/resource/scripts/rtisetenv_x64Linux3gcc5.4.0.bash && /re/bin/re_node_manager ${args}") != 0) {
                                                            error("re_node_manager failed on Node: ${node_name} : ${container_id}")
                                                        }
                                                    }
                                                } else {
                                                    //Run re_node_manager
                                                    if(utils.runScript("${RE_PATH}/bin/re_node_manager ${args}") != 0){
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
