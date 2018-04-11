def PROJECT_NAME = 'test_re'
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

stage("Checkout"){
    node("master"){
        dir(PROJECT_NAME){
            checkout scm
        }
        stash includes: "**", name: "source_code"
    }
}

def step_build_test = [:]
def step_test = [:]

def re_nodes = utils.getLabelledNodes("builder")
for(n in re_nodes){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/bin"){
                // Prune old bin
                deleteDir()
            }
            dir(PROJECT_NAME + "/lib"){
                // Prune old lib
                deleteDir()
            }
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //remove deleteDir for performance
                deleteDir()
                //Build the entire project 
                def success = utils.buildProject("Ninja", "-DBUILD_TEST=ON")
                if(!success){
                    // If we failed, try clear out the folder and build again
                    deleteDir()
                    success = utils.buildProject("Ninja", "-DBUILD_TEST=ON")
                }
                if(!success){
                    error("Cannot Compile")
                }
            }
        }
    }

    step_test[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/bin/test"){
                def globstr = "*"

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
                    }
                    stash includes: "*.xml", name: node_name + "_test_cases"
                }
                //Clean up the directory after
                deleteDir()
            }
        }
    }
}

stage("Build"){
    parallel step_build_test
}
stage("Test"){
    parallel step_test
}

node("master"){
    dir("test_cases"){
        for(n in re_nodes){
            unstash(n + "_test_cases")
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