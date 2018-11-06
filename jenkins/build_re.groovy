def PROJECT_NAME = 'test_re'
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils
import hudson.tasks.test.AbstractTestResultAction
def utils = new Utils(this);

// Override this to enable running of long tests
final Boolean IS_TAG = env.TAG_NAME
final GIT_CREDENTIAL_ID = "cditma-github-auth"
final GIT_ID = IS_TAG ? env.TAG_NAME : env.BRANCH_NAME
//Run full tests on a Tag or a Pull Request
final def RUN_ALL_TESTS = IS_TAG || GIT_ID.contains("PR-")
def RELEASE_DESCRIPTION = "re-" + GIT_ID

@NonCPS
def get_test_status(){
    //Thanks to https://stackoverflow.com/questions/39920437/how-to-access-junit-test-counts-in-jenkins-pipeline-project
    def status = "#Test Status\n"
    AbstractTestResultAction testResultAction = currentBuild.rawBuild.getAction(AbstractTestResultAction.class)
    if (testResultAction != null) {
        def total = testResultAction.totalCount
        def failed = testResultAction.failCount
        def skipped = testResultAction.skipCount
        def passed = total - failed - skipped
        status += "Passed: ${passed}, Failed: ${failed} ${testResultAction.failureDiffString}, Skipped: ${skipped}"
    }
    return status
}

stage("Checkout"){
    node("master"){
        dir(PROJECT_NAME){
            checkout scm
            stash includes: "**", name: "source_code"

            utils.runScript('git bundle create re.bundle origin/' + GIT_ID)
            utils.runScript('git-archive-all re.tar.gz')
            
            //Read the VERSION.MD
            if(fileExists("VERSION.md")){
                RELEASE_DESCRIPTION = readFile("VERSION.md")
            }
            final ROLLOUT_FILE_NAME = "re-" + GIT_ID + "-rollout.tar.gz"
            //Create rollout archive
            utils.runScript('tar -czf ' + ROLLOUT_FILE_NAME + ' re.bundle re.tar.gz')
            archiveArtifacts(ROLLOUT_FILE_NAME)
            deleteDir()
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

    node("master"){
        dir("test_cases"){
            deleteDir()

            for(n in builder_nodes){
                def node_name = n
                def stash_name = node_name + "_test_cases"
                unstash stash_name
            }

            def globstr = "**.xml"
            junit globstr
            
            //Test cases
            def test_archive = "test_results.zip"
            zip glob: globstr, zipFile: test_archive
            archiveArtifacts test_archive
        }
    }
}

if(IS_TAG){
    stage("Release"){
        node("master"){
            withCredentials([usernamePassword(credentialsId: GIT_CREDENTIAL_ID, passwordVariable: 'GITHUB_TOKEN', usernameVariable: 'GIT_USERNAME')]){
                def release_name = "re " + GIT_ID
                def git_args = " --user cdit-ma --repo re --tag " + GIT_ID

                //Write a VERSION.md to be used as the Git Release Description
                writeFile(file: "VERSION.md", text: RELEASE_DESCRIPTION + "\n" + get_test_status())

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

//Collate Results
node("master"){
    if(FAILED){
        print("Test Execution failed!")
        print(FAILURE_LIST.size() + " Error(s)")
        for(failure in FAILURE_LIST){
            print("ERROR: " + failure)
        }
        error("Test Execution failed!")
    }
}



