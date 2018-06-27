def PROJECT_NAME = 'test_re'
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

node("master"){
    stage("Checkout"){
        dir(PROJECT_NAME){
            checkout scm
            stash includes: "**", name: "source_code"
        }
    }
}

def builder_map = [:]
def test_map = [:]
def builder_nodes = utils.getLabelledNodes("builder")

if(builder_nodes.size() == 0){
    error('Cannot find any builder nodes.')
}

//Construct a builder map
for(n in builder_nodes){
    def node_name = n

    builder_map[node_name] = {
        node(node_name){
            dir(PROJECT_NAME){
                dir("bin"){
                    deleteDir()
                }
                dir("lib"){
                    deleteDir()
                }

                //Unstash the code
                unstash name: "source_code"
                
                dir("build"){
                    if(!utils.buildProject("Ninja", "-DBUILD_TEST=ON")){
                        error("CMake failed on Builder Node: " + node_name)
                    }
                }
            }
        }
    }

    test_map[node_name] = {
        node(node_name){
            def path = "${PATH}"
            //Append bin and lib directories for this build of re into the path for windows to find dlls
            def ABS_RE_PATH = pwd() + "/" + PROJECT_NAME + "/"
            path = ABS_RE_PATH + "bin;" + ABS_RE_PATH + "lib;" + path

            //Find all executables
            def test_list = findFiles glob: globstr

            def test_count = 0;
            def test_error_count = 0;

            dir("results"){
                for(def file : test_list){
                    def file_path = file.name
                    def file_name = utils.trimExtension(file_path)
                    def test_output = file_name + "_" + node_name + ".xml"
                    print("Running Test: " + file_path)
                    def test_error_code = utils.runScript("../" + file_path + " --gtest_output=xml:" + test_output)

                    if(test_error_code != 0){
                        print("Test: " + file_path + " Failed!")
                        currentBuild.result = 'FAILURE'
                    }
                    def stash_name = node_name + "_test_cases"
                    stash includes: "*.xml", name: stash_name, allowEmpty: true
                    //Clean up the directory after
                    deleteDir()
                }
                stash includes: "*.xml", name: node_name + "_test_cases"
            }
            //Clean up the directory after
            deleteDir()
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
    dir("test_cases"){
        for(n in builder_nodes){
            def node_name = n
            def stash_name = node_name + "_test_cases"
            unstash name: stash_name, allowEmpty: true
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
        deleteDir()
    }
}