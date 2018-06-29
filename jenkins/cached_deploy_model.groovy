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
            def build_dir = stash_name

            dir(build_dir){
                def current_file_hash = [:]
                def cached_dir = experiment_name
                def temp_dir = build_id

                dir(cached_dir){
                    touch "lock"
                    for(file in findFiles(glob: '**')){
                        //Calculate the hash of the files that exist
                        def file_sha = sha1(file.path)
                        current_file_hash[file.path] = file_sha
                    }
                }

                def files_to_copy = []
                //Code may already be here
                dir(temp_dir){
                    print("Entering Temp Dir: " + temp_dir)
                    //Unstash the generated code
                    unstash 'codegen'

                    for(file in findFiles(glob: '**')){
                        def copy_file = true
                        //Calculate the hash of the files that exist

                        def file_sha = sha1(file.path)
                        if(current_file_hash.containsKey(file.path)){
                            if(file_sha == current_file_hash[file.path]){
                                copy_file = false
                            }
                        }

                        if(copy_file){
                            files_to_copy += file.path
                        }
                    }
                }
                for(file in files_to_copy){
                    print("Copying: " + file)
                    def src_path = temp_dir + "/" + file
                    def dst_path = cached_dir + "/" + file
                    sh 'mkdir -p `dirname "' + dst_path + '"` && cp "' + src_path + '" "' + dst_path + '"'
                }

                dir(cached_dir){
                    dir("build"){
                        if(!utils.buildProject("Ninja", "")){
                            error("CMake failed on Builder Node: " + node_name)
                        }
                    }
                    dir("lib"){
                        //Stash all Libraries
                        stash includes: '**', name: stash_name
                        deleteDir()
                    }
                }
            }
        }
    }
}

//Run compilation scripts
stage("Compiling C++"){
    parallel builder_map
}
