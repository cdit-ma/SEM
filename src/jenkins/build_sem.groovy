#!groovy

// This isn't actually importing a jenkins library. Find information regarding jenkins shared libraries.
//  See sem/src/jenkins/Utils.groovy
@Library('jenkins') _
import hudson.tasks.test.AbstractTestResultAction
def utils = new jenkins.Utils(this)

final Boolean IS_TAG = env.TAG_NAME
final GIT_ID = IS_TAG ? env.TAG_NAME : env.BRANCH_NAME
//Run full tests on a Tag or a Pull Request
final Boolean RUN_PERFORMANCE_TESTS = IS_TAG || GIT_ID.contains("PR-") || GIT_ID.contains("release-")
def RELEASE_DESCRIPTION = "SEM-" + GIT_ID

final BUILD_PACKAGE = IS_TAG || GIT_ID.contains("PR-") || GIT_ID.contains("release-")
final UPLOAD_PACKAGE = IS_TAG

final CI_BUILD_NODES = nodesByLabel("ci_build_node")

final EXPECTED_PLATFORMS = ["Windows 10", "Linux", "Mac OS X"]

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
    agent{node "ci_build_node"}

    stages{
        stage("Pre-build environment checks"){
            steps{
                script{
                    def found_platforms = []
                    for(n in CI_BUILD_NODES){
                        def node_name = n
                        found_platforms << utils.getNodeOSName(node_name)
                    }

                    def all_platforms_found = EXPECTED_PLATFORMS.intersect(found_platforms).size() == EXPECTED_PLATFORMS.size()
                    if(!all_platforms_found){
                        error("Did not find all required platforms! Required platforms: ${EXPECTED_PLATFORMS} Found platforms: ${found_platforms}")
                    }
                }
            }
        }
        stage("Checkout/Bundle"){
            steps{
                script{
                    deleteDir()
                    checkout scm
                    stash includes: "**", name: "source_code"

                    def rollout_file = "sem-${GIT_ID}-rollout.tar.gz"

                    println rollout_file

                    if(utils.runScript("git archive -o ${rollout_file} HEAD") != 0){
                        error("Cannot create git archive")
                    }

                    //Read the VERSION.MD
                    if(fileExists("VERSION.md")){
                        RELEASE_DESCRIPTION = readFile("VERSION.md")
                    }

//                     //Create rollout archive
//                     if(utils.runScript("tar -czf ${rollout_file} sem.bundle sem.tar.gz") != 0){
//                         error("Cannot tar git archives")
//                     }
                    archiveArtifacts(rollout_file)
                }
            }
        }

        stage("Compilation"){
            steps{
                script{
                    def builder_map = [:]
                    for(n in CI_BUILD_NODES){
                        def node_name = n
                        builder_map[node_name] = {
                            node(node_name){
                                unstash "source_code"
                                dir("build"){
                                    deleteDir()
                                    def generate_args = "-DBUILD_TEST=ON"
                                    def os_name = utils.getNodeOSName(node_name)
                                    if(env.SEM_DEPS_CACHE_DIR){
                                        generate_args = generate_args + " -DSEM_DEPS_BASE_DIR=${env.SEM_DEPS_CACHE_DIR}"
                                    }
                                    if(!utils.buildProject("Ninja", generate_args)){
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
                    for(n in CI_BUILD_NODES){
                        def node_name = n

                        test_map[node_name] = {
                            node(node_name){
                                def re_path = pwd()
                                def test_dir = "build/bin/test/gtest"
                                // If windows, use top level binary directory s.t. we don't run into dll linking issues.
                                // TODO: Change tests to build into libs and run through single "test runner" binary
                                if(!isUnix()) {
                                    test_dir = "build/bin"
                                }
                                dir(test_dir){
                                    def os_name = utils.getNodeOSName(node_name)

                                    def glob_str = "test_*"
                                    if(os_name == "Windows 10"){
                                        glob_str = "test_*.exe"
                                    }

                                    def tests_list = findFiles glob: glob_str

                                    def environment_manager_flags = "-e ${env.ENVIRONMENT_MANAGER_ADDRESS} -n ${node_name}"
                                    dir("results"){
                                        for(f in tests_list){
                                            def file_path = f.name
                                            def file_name = utils.trimExtension(file_path)
                                            def test_output = "${file_name}_${node_name}.xml"
                                            print("Running Test: ${file_path}")

                                            def test_filter = ""
                                            if(!RUN_PERFORMANCE_TESTS){
                                                // TODO: Add *Performance* filter
                                                test_filter = " --gtest_filter=-*_LONG*"
                                            }

                                            if(utils.runScript("../${file_path} --gtest_output=xml:${test_output} ${test_filter} ${environment_manager_flags}") != 0){
                                                error("Running Test: ${file_path} Failed!")
                                            }
                                        }
                                        def stash_name = "${node_name}_test_cases"
                                        stash includes: "*.xml", name: stash_name, allowEmpty: true
                                        //Clean up the directory after
                                        deleteDir()
                                    }
                                }
//                                 dir("re/test/re_gen"){
//                                     def test_suite_xml = "${node_name}_regen_tests.xml"
//
//                                     if(utils.runScript("python run_tests.py -o ${test_suite_xml} --re_path ${re_path}") != 0){
//                                         error("Running run_tests.py Failed!")
//                                     }
//
//                                     junit test_suite_xml
//                                     dir("build"){
//                                         deleteDir()
//                                     }
//                                 }
                                // TODO: add qt tests
                                //  live in bin/test/qttest
                            }
                        }
                    }

                    //Run the Tests
                    parallel(test_map)

                    dir("test_cases"){
                        deleteDir()

                        //Unstash all test_cases
                        for(n in CI_BUILD_NODES){
                            try{
                                unstash  "${n}_test_cases"
                            }catch(Exception e){
                                //Ignore
                            }
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

        stage("Archive + Pack") {
            steps{
                script{
                    def step_archive = [:]
                    for(n in CI_BUILD_NODES){
                        def node_name = n
                        step_archive[node_name] = {
                            node(node_name){
                                dir("build"){
                                    def os_name = utils.getNodeOSName(node_name)

                                    def globstr = ""
                                    if(os_name == "Linux"){
                                        //Dont run package for linux nodes
                                        return;
                                    }else if(os_name == "Mac OS X"){
                                        globstr = '*.dmg'
                                    }else if(os_name == "Windows 10"){
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
                    if(BUILD_PACKAGE) {
                        parallel(step_archive)
                    }
                }
            }
        }
    }
}
