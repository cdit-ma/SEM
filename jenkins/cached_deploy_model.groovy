//This script requires the following Jenkins plugins:
//-Pipeline: Utility Steps

//Requires following parameters in jenkins job:
// -String parameter: MASTER_NODE
// -String parameter: EXECUTION_TIME
// -String parameter: EXPERIMENT_NAME
// -String parameter: ENVIRONMENT_MANAGER_ADDRESS
//
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final master_node = "${MASTER_NODE}"
final execution_time = "${EXECUTION_TIME}"
final env_manager_addr = "${ENVIRONMENT_MANAGER_ADDRESS}"
final build_id = env.BUILD_ID
final CLEANUP = false

def experiment_name = "${EXPERIMENT_NAME}"
if(experiment_name.isEmpty()){
    experiment_name = "deploy_model_" + build_id
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

//Get one node of each kind
for(builder_type in ["builder_centos7", "builder_ubuntu18"]){
    for(node_name in nodesByLabel(builder_type)){
        builder_nodes += node_name
        break;
    }
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
    

    stage('C++ Generation'){
        dir(build_id + "/Codegen"){
            unstash 'model'
            sleep(1)
            
            def re_gen_path = '${RE_PATH}/re_gen/'
            def saxon_call = 'java -jar ' + re_gen_path + '/saxon.jar -xsl:' + re_gen_path
            def file_parameter = ' -s:' + MODEL_FILE
            
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
        node(node_name){
            def stash_name = "code_" + utils.getNodeOSVersion(node_name)
            def new_stash_name = "new_" + stash_name
            def build_dir = stash_name

            dir(build_dir){
                def cached_dir = experiment_name
                //Make a list of files to delete
                def files_to_remove = []

                dir(build_id){
                    //Unstash the generated code
                    unstash 'codegen'

                    for(file in findFiles(glob: '**')){
                        def file_path = file.path
                        def remove_file = false
                        
                        //Ignore graphml/zip files
                        for(ext in [".graphml", ".zip"]){
                            if(file_path.endsWith(ext)){
                                remove_file = true
                            }
                        }

                        if(!remove_file){
                            //Check to see if the corresponding file exists in the cached dir
                            def dst_path = "../" + cached_dir + "/" + file_path
                            if(fileExists(dst_path)){
                                def src_file_sha = sha1(file_path)
                                def dst_file_sha = sha1(dst_path)
                                if(src_file_sha == dst_file_sha){
                                    remove_file = true
                                }
                            }
                        }

                        if(remove_file){
                            files_to_remove += file_path
                        }
                    }

                    //Write all files to be removed into a file, then remove them in bash, hide the output
                    writeFile file: 'files_to_remove.list', text: files_to_remove.join("\n")
                    if(utils.runScript('while read file ; do rm "$file" ; done < files_to_remove.list  > /dev/null 2>&1') != 0){
                        print("Cannot remove files")
                    }
                    
                    //Stash only the different files
                    stash name: new_stash_name, allowEmpty: true
                    //Delete old directory
                    deleteDir()
                }
                
                dir(cached_dir){
                    unstash name: new_stash_name

                    dir("build"){
                        if(!utils.buildProject("Ninja", "")){
                            error("CMake failed on Builder Node: " + node_name)
                        }
                    }
                    dir("lib"){
                        //Stash all Libraries
                        stash includes: '**', name: stash_name
                        //Remove the binary directory once we are done
                        deleteDir()
                    }
                }
            }
        }
    }
}


//Produce the execution map
for(n in nodes){
    def node_name = n;
    execution_map[node_name] = {
        node(node_name){
            dir(build_id){
                //Get the IP of this node
                def ip_addr = utils.getNodeIpAddress(node_name)
                
                dir("lib"){
                    //Unstash the required libraries for this node.
                    //Have to run in the lib directory due to dll linker paths
                    unstash "code_" + utils.getNodeOSVersion(node_name)
                    
                    def args = ' -n "' + experiment_name + '"'
                    args += " -e " + env_manager_addr
                    args += " -a " + ip_addr
                    args += " -l ."

                    def logan_args = ' -n "' + experiment_name + '"'
                    logan_args += " -e " + env_manager_addr
                    logan_args += " -a " + ip_addr

                    if(node_name == master_node){
                        unstash 'model'
                        args += " -t " + execution_time
                        args += ' -d "' + MODEL_FILE  + '"'
                    }

                    parallel(
                        ("LOGAN_" + node_name): {
                            //Run Logan
                            if(utils.runScript("${RE_PATH}/bin/logan_managedserver" + logan_args) != 0){
                                FAILURE_LIST << ("Logan failed on node: " + node_name)
                                FAILED = true
                            }
                        },
                        ("RE_" + node_name): {
                            //Run re_node_manager
                            if(utils.runScript("${RE_PATH}/bin/re_node_manager" + args) != 0){
                                FAILURE_LIST << ("Experiment slave failed on node: " + node_name)
                                FAILED = true
                            }
                        }
                    )
                    
                    //Archive any sql databases produced
                    if(findFiles(glob: '**/*.sql').size() > 0){
                        archiveArtifacts artifacts: '**/*.sql'
                    }
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
stage("Compiling C++"){
    parallel builder_map
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
    error("Model Execution failed!")
}