def PROJECT_NAME = 'test_medea'

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
def step_archive = [:]

def medea_nodes = utils.getLabelledNodes("MEDEA")
for(n in medea_nodes){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //Build the entire project 
                utils.buildProject("Ninja", "-DBUILD_APP=ON -DBUILD_CLI=ON")
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

stage("Package"){
    parallel step_archive
}