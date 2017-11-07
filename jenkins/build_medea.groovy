def PROJECT_NAME = 'test_medea'

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

def step_build_test = [:]
def step_test = [:]
def step_archive = [:]

def medea_nodes = utils.getLabelledNodes("MEDEA")
for(n in medea_nodes){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //Build the entire project 
                utils.buildProject("Ninja", "-DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_CLI=ON")
            }
        }
    }

    step_test[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/test/bin"){
                def globstr = "*"
                if(!isUnix()){
                    globstr = '*.exe'
                }

                //Find all executables
                def test_list = findFiles glob: globstr

                def test_count = 0;
                def test_error_count = 0;

                dir("results"){
                    for(def file : test_list){
                        def file_path = file.name
                        def file_name = trimExtension(file_path)
                        def test_output = file_name + "_" + node_name + ".xml"
                        print("Running Test: " + file_path)
                        //Launch the output test cases, in a folder
                        def test_error_code = runScript("../" + file_path + " -o " + test_output + ",xunitxml")

                        if(test_error_code != 0){
                            test_error_count ++
                        }
                    }
                    stash includes: "*.xml", name: node_name + "_test_cases"

                    //Clean up the directory after
                    deleteDir()
                }
            }
        }
    }

    step_archive[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/build"){
                utils.runScript("cpack")
                dir("installers"){
                    def globstr = ""

                    if(isUnix()){
                        globstr = '*.dmg'
                    }else{
                        globstr = '*.exe'
                    }

                    def file_list = findFiles glob: globstr

                    def archiveName = utils.trimExtension(file_list[0].name) + "-installer.zip"
                    zip glob: globstr, zipFile: archiveName

                    archiveArtifacts "*.zip"
                    
                    //Clean up the directory after
                    deleteDir()
                }
            }
        }
    }
}

stage("Build"){
    parallel step_build_test
}

stage(name: "Test"){
    parallel step_test
}

stage("Package"){
    parallel step_archive
}

node("master"){
    dir("test_cases"){
        for(n in medea_nodes){
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