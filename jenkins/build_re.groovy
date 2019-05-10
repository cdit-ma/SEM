#!groovy
@Library('cditma-utils') _
import hudson.tasks.test.AbstractTestResultAction
def utils = new cditma.Utils(this)

final Boolean IS_TAG = env.TAG_NAME
final GIT_ID = IS_TAG ? env.TAG_NAME : env.BRANCH_NAME
//Run full tests on a Tag or a Pull Request
final RUN_ALL_TESTS = IS_TAG || GIT_ID.contains("PR-") || GIT_ID.contains("release-")
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
                        deleteDir()
                        checkout scm
                        stash includes: "**", name: "source_code"

                        //Create a special branch called jenkins-rollout, pointed at this revision
                        if(utils.runScript("git checkout -b jenkins-rollout") != 0){
                            error("Cannot create git branch jenkins-rollout")
                        }
                        
                        if(utils.runScript("git bundle create re.bundle HEAD --branches") != 0){
                            error("Cannot create git bundle")
                        }

                        if(utils.runScript('git-archive-all re.tar.gz') != 0){
                            error("Cannot create git archive")
                        }

                        //Read the VERSION.MD
                        if(fileExists("VERSION.md")){
                            RELEASE_DESCRIPTION = readFile("VERSION.md")
                        }

                        def rollout_file = "re-${GIT_ID}-rollout.tar.gz"

                        //Create rollout archive
                        if(utils.runScript("tar -czf ${rollout_file} re.bundle re.tar.gz") != 0){
                            error("Cannot tar git archives")
                        }
                        archiveArtifacts(rollout_file)
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
                                def re_path = pwd()
                                dir("bin/test"){
                                    def glob_str = "test_*"
                                    if(!isUnix()){
                                        //If windows search for exe only
                                        glob_str += ".exe"
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
                                            if(!RUN_ALL_TESTS){
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
                                if(RUN_ALL_TESTS){
                                    dir("test/re_gen"){
                                        def test_suite_xml = "${node_name}_regen_tests.xml"

                                        if(utils.runScript("python run_tests.py ${test_suite_xml}") != 0){
                                            error("Running run_tests.py Failed!")
                                        }
                                        
                                        junit test_suite_xml
                                        dir("build"){
                                            deleteDir()
                                        }
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

        stage("Release Upload"){
            when{buildingTag()}
            steps{
                node("master"){
                    script{
                        withCredentials([usernamePassword(credentialsId: "cditma-github-auth", passwordVariable: 'GITHUB_TOKEN', usernameVariable: 'GIT_USERNAME')]){
                            def release_name = "re ${GIT_ID}"
                            def git_args = "--user cdit-ma --repo re --tag ${GIT_ID}"

                            //Write a VERSION.md to be used as the Git Release Description
                            writeFile(file: "VERSION.md", text: "${RELEASE_DESCRIPTION}\n\n" + get_test_status())

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
                                        utils.runScript("github-release upload ${git_args} -R --file \"${file_path}\" --name \"${file_path}\"")
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
}