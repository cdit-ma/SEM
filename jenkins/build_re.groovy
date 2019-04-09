#!groovy
@Library('cditma-utils') _
import groovy.time.TimeCategory 
import groovy.time.TimeDuration
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
                                    dir("test/re_gen/models"){
                                        def glob_str = "*.graphml"
                                        def graphml_list = findFiles glob: glob_str

                                        def test_count = 0;
                                        def fail_count = 0;
                                        def test_time = 0;

                                        def test_suites = ""
                                        def test_suite_name = "${node_name}_regen_tests"

                                        for(f in graphml_list){
                                            def file_path = f.name
                                            def file_name = utils.trimExtension(file_path)
                                            def test_name = file_name

                                            print("Running re_gen Test: ${test_name}")

                                            dir(test_name){
                                                def r_start = new Date()
                                                def regen_success = run_regen_xsl("${re_path}/re_gen", 'generate_project.xsl', "../${file_path}")
                                                def r_end = new Date()

                                                def build_success = false
                                                if(regen_success){
                                                    dir("build"){
                                                        build_success = utils.buildProject("Ninja", "")
                                                    }
                                                }
                                                def c_end = new Date()

                                                def regen_time = TimeCategory.minus(r_end, r_start).toMilliseconds() / 1000.0
                                                def compile_time = TimeCategory.minus(c_end, r_end).toMilliseconds() / 1000.0

                                                def t_time = regen_time + compile_time
                                                def t_fail_count = (regen_success ? 0 : 1) + (build_success ? 0 : 1)
                                                def suite_output = get_test_suite_open("Regen_${test_name}", 2, t_fail_count, t_time)
                                                suite_output += get_test_case("xsl_generation", regen_success, regen_time)
                                                suite_output += get_test_case("compilation", build_success, compile_time)
                                                suite_output += get_test_suite_close()
                                                
                                                //Update our top level counters
                                                test_suites += suite_output;
                                                test_count += 2
                                                fail_count += t_fail_count
                                                test_time += t_time
                                                //Clean up the directory after
                                                deleteDir()
                                            }
                                        }
                                        junit_output = get_xml_preamble()
                                        junit_output += get_test_suites_open("Regen_Tests", test_count, fail_count, test_time)
                                        junit_output += test_suites
                                        junit_output += get_test_suites_close()
                                        
                                        writeFile file: "${test_suite_name}.xml", text: junit_output
                                        junit  "${test_suite_name}.xml"
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




def run_regen_xsl(regen_path, transform, file_path, args = ""){
    def utils = new cditma.Utils(this)
    return utils.runScript("java -jar ${regen_path}/saxon.jar -xsl:${regen_path}/${transform} -s:${file_path} ${args}") == 0
}

def get_test_case(test_name, test_pass, test_time, fail_string = ""){
    def status = 'run'
    def testcase_args = "name=\"${test_name}\" status=\"${status}\" time=\"${test_time}\""
    def tab = "\t\t"
    def output = ""
    if(test_pass){
        output += "${tab}<testcase ${testcase_args} />\n"
    }else{
        output += "${tab}<testcase ${testcase_args}>\n"
        output += "${tab}\t<failure message=\"${fail_string}\" />\n"
        output += "${tab}</testcase>\n"
    }
    return output;
}

def get_test_suite_open(suite_name, test_count, failure_count, test_time){
    def errors = 0
    def disabled = 0
    return "\t<testsuite name=\"${suite_name}\" tests=\"${test_count}\" failures=\"${failure_count}\" errors=\"${errors}\" disabled=\"${disabled}\" time=\"${test_time}\">\n"
}

def get_test_suite_close(){
    return "\t</testsuite>\n"
}

def get_test_suites_open(suite_name, test_count, failure_count, test_time){
    def errors = 0
    def disabled = 0
    return "<testsuites name=\"${suite_name}\" tests=\"${test_count}\" failures=\"${failure_count}\" errors=\"${errors}\" disabled=\"${disabled}\" time=\"${test_time}\">\n"
}

def get_xml_preamble(){
    return '<?xml version="1.0" encoding="UTF-8"?>\n'
}

def get_test_suites_close(){
    return "</testsuites>\n"
}