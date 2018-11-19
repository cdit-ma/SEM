#!groovy
@Library('cditma-utils') _
import hudson.tasks.test.AbstractTestResultAction
def utils = new cditma.Utils(this)

final Boolean IS_TAG = env.TAG_NAME
final GIT_ID = IS_TAG ? env.TAG_NAME : env.BRANCH_NAME
//Run full tests on a Tag or a Pull Request
final RUN_ALL_TESTS = IS_TAG || GIT_ID.contains("PR-")
def RELEASE_DESCRIPTION = "re-" + GIT_ID

@NonCPS
def get_test_status(){
    //Thanks to https://stackoverflow.com/questions/39920437/how-to-access-junit-test-counts-in-jenkins-pipeline-project
    AbstractTestResultAction test = currentBuild.rawBuild.getAction(AbstractTestResultAction.class)
    if(test != null){
        def total = test.totalCount
        def passed = total - test.failCount - test.skipCount
        return "Passed ${passed}/${total} Tests"
    }
    return ""
}


pipeline{
    agent{node "builder"}

    stages{
        stage("Checkout/Bundle"){
            steps{
                node("master"){
                    script{
                        checkout scm
                        stash includes: "**", name: "source_code"
                        
                        //Get the SHA
                        def COMMIT_SHA = utils.runScript('git rev-parse HEAD', false)
                        //Checkout into master
                        utils.runScript('git branch master')
                        utils.runScript('git checkout master')
                        //point master at the SHA of this branch/tag
                        utils.runScript("git reset --hard ${COMMIT_SHA}")

                        utils.runScript('git bundle create re.bundle master --tags --branches')
                        utils.runScript('git-archive-all re.tar.gz')

                        //Read the VERSION.MD
                        if(fileExists("VERSION.md")){
                            RELEASE_DESCRIPTION = readFile("VERSION.md")
                        }

                        def rollout_file = "re-${GIT_ID}-rollout.tar.gz"

                        //Create rollout archive
                        utils.runScript("tar -czf ${rollout_file} re.bundle re.tar.gz")
                        archiveArtifacts(rollout_file)
                        deleteDir()
                    }
                }
            }
        }

        stage("Compilation"){
            steps{
                script{
                    def builder_map = [:]
                    for(n in nodesByLabel("builder")){
                        def node_name = n

                        builder_map[node_name] = {
                            node(node_name){
                                dir("bin"){
                                    deleteDir()
                                }
                                dir("lib"){
                                    deleteDir()
                                }

                                unstash "source_code"
                                dir("build"){
                                    deleteDir()
                                    if(!utils.buildProject("Ninja", "-DBUILD_TEST=ON")){
                                        error("CMake failed on Builder Node: ${node_name}")
                                    }
                                }
                            }
                        }
                    }
                    parallel(builder_map)
                }
            }
        }

        stage("Execute Test"){
            steps{
                script{
                    def test_map = [:]
                    for(n in nodesByLabel("builder")){
                        def node_name = n

                        test_map[node_name] = {
                            node(node_name){
                                dir("bin/test"){
                                    def glob_str = "test_*"
                                    if(!isUnix()){
                                        //If windows search for exe only
                                        glob_str += ".exe"
                                    }
                                        
                                    def tests_list = findFiles glob: glob_str
                                    dir("results"){
                                        for(f in tests_list){
                                            def file_path = f.name
                                            def file_name = utils.trimExtension(file_path)
                                            def test_output = "${file_name}_${node_name}.xml"
                                            print("Running Test: ${file_path}")

                                            def test_filter = ""
                                            if(!RUN_ALL_TESTS){
                                                test_filter = " --gtest_filter=-*LONG_*"
                                            }

                                            if(utils.runScript("../${file_path} --gtest_output=xml:${test_output} ${test_filter}") != 0){
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
                    }
                    //Run the Tests
                    parallel(test_map)

                    dir("test_cases"){
                        deleteDir()
                        
                        //Unstash all test_cases
                        for(n in nodesByLabel("builder")){
                            unstash  "${n}_test_cases"
                        }

                        def glob_str = "**.xml"
                        junit glob_str

                        //Test cases
                        def test_archive = "test_results.zip"
                        zip glob: glob_str, zipFile: test_archive
                        archiveArtifacts test_archive

                        //Set the Display Name
                        currentBuild.displayName = "#${env.BUILD_ID} - " + get_test_status()
                    }
                }
            }
        }

        stage("Release Upload"){
            when{buildingTag()}
            steps{
                script{
                    withCredentials([usernamePassword(credentialsId: "cditma-github-auth", passwordVariable: 'GITHUB_TOKEN', usernameVariable: 'GIT_USERNAME')]){
                        def release_name = "re " + GIT_ID
                        def git_args = "--user cdit-ma --repo re --tag " + GIT_ID

                        //Write a VERSION.md to be used as the Git Release Description
                        writeFile(file: "VERSION.md", text: RELEASE_DESCRIPTION + "\n\n" + get_test_status())

                        def release_success = utils.runScript("github-release release ${git_args} --name \"${release_name}\" -d - < VERSION.md") == 0
                        if(!release_success){
                            release_success = utils.runScript("github-release edit ${git_args} --name \"${release_name}\" -d - < VERSION.md") == 0
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
}