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
            def RE_PATH = pwd() + "/" + PROJECT_NAME
            def RE_LIB_PATH = RE_PATH + "/lib"
            def env_vars = []
            
            if(isUnix()){
                //SET LD_LIBRARY_PATH to force the linker to find this projects libraries
                env_vars += "LD_LIBRARY_PATH=" + RE_LIB_PATH + ":$LD_LIBRARY_PATH"
            }else{
                //SET PATH to force the linker to find this projects libraries
                env_vars += "PATH=" + RE_LIB_PATH + ":$PATH"
            }

            withEnv(env_vars){
                dir(RE_PATH + "/bin/test"){
                    print("LD_LIBRARY_PATH:$LD_LIBRARY_PATH")

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
                            def test_error_code = utils.runScript("../" + file_path + " --gtest_output=xml:" + test_output)

                            if(test_error_code != 0){
                                print("Test: " + file_path + " Failed!")
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