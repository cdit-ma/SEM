def PROJECT_NAME = 'test_medea'

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

final GIT_BRANCH = env.JOB_BASE_NAME
final FILE_NAME = "MEDEA-" + GIT_BRANCH


stage("Checkout"){
    node("master"){
        dir(PROJECT_NAME){
            checkout scm
        }
        stash includes: "**", name: "source_code"

        def git_tar_gz = FILE_NAME + '-src.tar.gz'
        dir(PROJECT_NAME){
            utils.runScript('git-archive-all ../' + git_tar_gz)
        }
        archiveArtifacts(git_tar_gz)
    }
}

def step_build_test = [:]
def step_archive = [:]

def medea_nodes = nodesByLabel("build_medea")
for(n in medea_nodes){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //Build the entire project 
                def success = utils.buildProject("Ninja", "-DBUILD_APP=ON -DBUILD_CLI=ON")

                if(!success){
                    error("CMake failed on Builder Node: " + node_name)
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
                    
                    def os_name = utils.getNodeOSName(node_name)

                    if(os_name == "Linux"){
                        globstr = '*.run'
                    }else if(os_name == "Mac OS X"){
                        globstr = '*.dmg'
                    }else if(os_name == "Windows"){
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