//This script requires the following Jenkins plugins:
//-Pipeline: Utility Steps

//Requires following parameters in jenkins job:
// -String parameter: MASTER_NODE
// -String parameter: EXECUTION_TIME
// -String parameter: EXPERIMENT_NAME
// -String parameter: ENVIRONMENT_MANAGER_ADDRESS

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

final master_node = "${MASTER_NODE}"
final execution_time = "${EXECUTION_TIME}"
final env_manager_addr = "${ENVIRONMENT_MANAGER_ADDRESS}"
final build_id = env.BUILD_ID

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
final MODEL_FILE = experiment_name + "_" + build_id + ".graphml"

def builder_nodes = []
def nodes = utils.getLabelledNodes("re")

//Get one node of each kind
for(builder_type in ["builder_centos7", "builder_ubuntu18"]){
    for(node_name in utils.getLabelledNodes(builder_type)){
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
    //Stash the model file
    stash includes: MODEL_FILE, name: 'model'

    stage('C++ Generation'){
        dir(build_id + "/Codegen"){
            unstash 'model'
            def re_gen_path = '${RE_PATH}/re_gen/'
            def saxon_call = 'java -jar ' + re_gen_path + '/saxon.jar -xsl:' + re_gen_path
            def file_parameter = ' -s:' + MODEL_FILE
            
            def run_type_gen = saxon_call + '/g2datatypes.xsl' + file_parameter
            def run_components_gen = saxon_call + '/g2components.xsl' + file_parameter
            def run_validation = saxon_call + '/g2validate.xsl' + file_parameter + ' write_file=true'

            if(utils.runScript(run_type_gen) != 0){
                error('Datatype code generation failed.')
            }

            if(utils.runScript(run_components_gen) != 0){
                error('Components code generation failed.')
            }

            if(utils.runScript(run_validation) != 0){
                error('Validation report generation failed.')
            }

            //Construct a zip file with the code
            zip(zipFile: '../codegen.zip', archive: true)

            //Stash the generated code
            stash includes: '**', name: 'codegen'
            
            //Archive the model
            archiveArtifacts MODEL_FILE
            //Archive the validation report
            archiveArtifacts 'validation_report.xml'
            
            

            //Delete the Dir
            deleteDir()
        }
    }
}

//Run Compilation
for(node_name in builder_nodes){
    //Define the Builder instructions
    builder_map[node_name] = {
        node(node_name){
            def stash_name = "code_" + utils.getNodeOSVersion(node_name)
            dir(build_id + "/" + stash_name){
                //Unstash the generated code
                unstash 'codegen'

                dir("build"){
                    if(!utils.buildProject("Ninja", "")){
                        error("CMake failed on Builder Node: " + node_name)
                    }
                }
                dir("lib"){
                    //Stash all Libraries
                    stash includes: '**', name: stash_name
                }
            }
            //Delete the Dir
            //deleteDir()
        }
    }
}

//Produce the execution map
for(node_name in nodes){
    execution_map[node_name] = {
        node(node_name){
            dir(build_id){
                //Get the IP of this node
                def ip_addr = utils.getNodeIpAddress(node_name)
                
                dir("lib"){
                    //Unstash the required libraries for this node
                    unstash "code_" + utils.getNodeOSVersion(node_name)
                }
                def args = ""
                args += " -n " + experiment_name
                args += " -e " + env_manager_addr
                args += " -a " + ip_addr
                args += " -l lib"
                    
                if(node_name == master_node){
                    unstash 'model'
                    args += " -t " + execution_time
                    args += " -d " + MODEL_FILE
                }

                //Run re_node_manager
                if(utils.runScript("${RE_PATH}/bin/re_node_manager" + args) != 0){
                    FAILURE_LIST << ("Experiment slave failed on node: " + node_name)
                    FAILED = true
                }

                /*
                LOGAN PARAMETERS
                def logan_args = ""
                logan_args += " -n " + experiment_name
                logan_args += " -e " + env_manager_addr
                logan_args += " -a " + ip_addr
                if(utils.runScript("${RE_PATH}/../logan/bin/logan_clientserver" + logan_args) != 0){
                    FAILURE_LIST << ("Experiment slave failed on node: " + node_name)
                    FAILED = true
                }*/
                deleteDir()
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
