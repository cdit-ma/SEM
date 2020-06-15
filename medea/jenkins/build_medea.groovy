//This script requires the following Jenkins plugins:
//-Pipeline: Utility Steps
//Requires a Jenkins Credential with id: cditma-github-auth

def PROJECT_NAME = 'build_medea'
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

final GIT_CREDENTIAL_ID = "cditma-github-auth"
final Boolean IS_TAG = env.TAG_NAME
final GIT_ID = IS_TAG ? env.TAG_NAME : env.BRANCH_NAME
final FILE_NAME = "MEDEA-" + GIT_ID
final BUILD_PACKAGE = IS_TAG || GIT_ID.contains("PR-") || GIT_ID.contains("release-")
final UPLOAD_PACKAGE = IS_TAG

def RELEASE_DESCRIPTION = FILE_NAME

stage("Checkout"){
    node("master"){
        dir(PROJECT_NAME){
            checkout scm
        }
        stash includes: "**", name: "source_code"

        def git_tar_gz = FILE_NAME + '-src.tar.gz'
        dir(PROJECT_NAME){
            utils.runScript('git-archive-all ../' + git_tar_gz)

            //Read the VERSION.MD
            if(fileExists("VERSION.md")){
                RELEASE_DESCRIPTION = readFile("VERSION.md")
            }
        }
        archiveArtifacts(git_tar_gz)
    }
}

def step_build = [:]
def step_test = [:]
def step_archive = [:]

def medea_nodes = nodesByLabel("build_medea")
for(n in medea_nodes){
    def node_name = n

    step_build[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //Build the entire project 
                def success = utils.buildProject("Ninja", "-DBUILD_APP=ON -DBUILD_CLI=ON -DBUILD_TEST=ON")

                if(!success){
                    error("CMake failed on Builder Node: " + node_name)
                }
            }
        }
    }

    step_test[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/build/bin/tests"){
                def glob_str = "test_*"
                if(!isUnix()){
                    //If windows search for exe only
                    glob_str += ".exe"
                }
                def test_dir = pwd()
                def tests_list = findFiles glob: glob_str
                dir("results"){
                    for(f in tests_list){
                        def file_path = test_dir + "/" + f.name
                        def file_name = utils.trimExtension(f.name)
                        def test_output = "${file_name}_${node_name}.xml"
                        print("Running Test: ${file_name}")

                        if(utils.runScript("${file_path} -xunitxml -o ${test_output}") != 0){
                            error("Running Test: ${file_path} Failed!")
                        }
                    }
                    def stash_name = "${node_name}_test_cases"
                    stash includes: "*.xml", name: stash_name, allowEmpty: true
                    //Clean up the directory after
                    deleteDir()
                }
            }
        }
    }

    step_archive[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/build"){
                def os_name = utils.getNodeOSName(node_name)

                def globstr = ""
                if(os_name == "Linux"){
                    //Dont run package for linux nodes
                    return;
                }else if(os_name == "Mac OS X"){
                    globstr = '*.dmg'
                }else if(os_name == "Windows"){
                    globstr = '*.exe'
                }

                utils.runScript("cpack")
                dir("installers"){
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
    parallel step_build
}

stage("Test"){
    parallel step_test

    node("master"){
        dir("test_cases"){
            deleteDir()
            for(n in medea_nodes){
                unstash  "${n}_test_cases"
            }

            def glob_str = "**.xml"
            junit glob_str
        }
    }
}

if(BUILD_PACKAGE){
    stage("Package"){
        parallel step_archive
    }

    if(UPLOAD_PACKAGE){
        stage("Release"){
            node("master"){
                withCredentials([usernamePassword(credentialsId: GIT_CREDENTIAL_ID, passwordVariable: 'GITHUB_TOKEN', usernameVariable: 'GIT_USERNAME')]) {
                    def release_name = "MEDEA " + GIT_ID
                    def git_args = " --user cdit-ma --repo MEDEA --tag " + GIT_ID

                    //Write a VERSION.md to be used as the Git Release Description
                    writeFile(file: "VERSION.md", text: RELEASE_DESCRIPTION)

                    def release_success = utils.runScript("github-release release" + git_args + " --name \"" + release_name + "\" -d - < VERSION.md") == 0
                    if(!release_success){
                        //Try edit if release fails
                        release_success = utils.runScript("github-release edit" + git_args + " --name \"" + release_name + "\" -d - < VERSION.md") == 0
                    }

                    if(release_success){
                        dir("binaries"){
                            unarchive mapping: ['*' : '.']
                            for(def file : findFiles(glob: '*')){
                                def file_path = file.name
                                //Upload binaries, replacing if they exist
                                utils.runScript("github-release upload" + git_args + " -R --file \"" + file_path + "\" --name \"" + file_path + "\"")
                            }
                        }
                    }else{
                        error("GitHub Release failed to be created")
                    }
                }
            }
        }
    }
}