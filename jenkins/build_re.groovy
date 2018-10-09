def PROJECT_NAME = 'test_re'
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

// Override this to enable running of long tests
def RUN_ALL_TESTS = false
final branch_name = env.BRANCH_NAME
if(branch_name.contains("PR-")){
    RUN_ALL_TESTS = true
}

node("master"){
    stage("Checkout"){
        dir(PROJECT_NAME){
            checkout scm
            stash includes: "**", name: "source_code"
        }
    }
}

def FAILED = false
def FAILURE_LIST = []

def builder_map = [:]
def test_map = [:]
def builder_nodes = nodesByLabel("builder")

if(builder_nodes.size() == 0){
    error('Cannot find any builder nodes.')
}

//Construct a builder map
for(n in builder_nodes){
    def node_name = n

    builder_map[node_name] = {
        node(node_name){
            dir(PROJECT_NAME){
                deleteDir()
            }
            dir(PROJECT_NAME){
                //Unstash the code
                unstash name: "source_code"

                dir("testybois"){
                    unstash name: "source_code"
                }
                
                dir("build"){
                    sh("env")
                    if(!utils.buildProject("Ninja", "-DBUILD_TEST=ON")){
                        error("CMake failed on Builder Node: " + node_name)
                    }
                }
            }
        }
    }

    test_map[node_name] = {
        node(node_name){
            def RE_PATH = pwd() + "/" + PROJECT_NAME
            dir(RE_PATH + "/bin/test"){
                def globstr = "test_*"
                if(!isUnix()){
                    //If windows search for exe only
                    globstr += ".exe"
                }

                //Find all executables
                def test_list = findFiles glob: globstr
                
                dir("results"){
                    for(def file : test_list){
                        def file_path = file.name
                        def file_name = utils.trimExtension(file_path)
                        def test_output = file_name + "_" + node_name + ".xml"
                        print("Running Test: " + file_path)
                        def test_filter = ""
                        if (!RUN_ALL_TESTS) {
                            test_filter = " --gtest_filter=-*LONG_*"
                        }
                        def test_error_code = utils.runScript("../" + file_path + " --gtest_output=xml:" + test_output + test_filter)

                    if(test_error_code != 0){
                            FAILED = true
                            print("Test: " + file_path + " Failed!")
                            FAILURE_LIST << ("Test "+file_path+" failed on node: " + node_name)
                        }
                    }
                    def stash_name = node_name + "_test_cases"
                    stash includes: "*.xml", name: stash_name, allowEmpty: true
                    //Clean up the directory after
                    deleteDir()
                }
            }
        }
    }
}

stage("Build"){
    parallel builder_map
}

stage("Test"){
    parallel test_map
}

//Collate Results
node("master"){
    stage("Collate"){
        dir("test_cases"){
            deleteDir()

            for(n in builder_nodes){
                def node_name = n
                def stash_name = node_name + "_test_cases"
                unstash stash_name
            }

            def globstr = "**.xml"
            def test_results = findFiles glob: globstr
            for (int i = 0; i < test_results.size(); i++){
                def file_path = test_results[i].name
                junit file_path
            }
            
            //Test cases
            def test_archive = "test_results.zip"
            zip glob: globstr, zipFile: test_archive
            archiveArtifacts test_archive
        }

        if(FAILED){
            print("Test Execution failed!")
            print(FAILURE_LIST.size() + " Error(s)")
            for(failure in FAILURE_LIST){
                print("ERROR: " + failure)
            }
            error("Test Execution failed!")
        }
    }
}
