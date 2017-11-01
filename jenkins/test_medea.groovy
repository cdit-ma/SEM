def PROJECT_NAME = 'test_medea'

// This method collects a list of Node names from the current Jenkins instance
@NonCPS
def nodeNames() {
  return jenkins.model.Jenkins.instance.nodes.collect { node -> node.name }
}

//Gets nodes label
def getLabels(String name){
    def computer = Jenkins.getInstance().getComputer(name)
    def node = computer.getNode()
    if(computer.isOnline()){
        return node.getLabelString()
    }
    return ""
}

//Get labelled nodes
def getLabelledNodes(String label){
    def filtered_names = []
    def names = nodeNames()
    for(n in nodeNames()){
        if(getLabels(n).contains(label)){
            filtered_names << n
        }
    }
    return filtered_names
}

//Run script, changes to bat if windows detected.
def runScript(String script){
    if(isUnix()){
        out = sh(returnStatus: true, script: script)
        return out
    }
    else{
        out = powershell(returnStatus:true, script: script)
        return out
    }
}

def buildProject(String generator, String cmake_options){
    print "Calling CMake generate"
    runScript("cmake .. -G " + generator + " -DCMAKE_BUILD_TYPE=Release " + cmake_options)
    print "Calling CMake --build"
    runScript("cmake --build . --config Release")
}

def trimExtension(String filename){
    def index = filename.lastIndexOf(".")
    if (index > 0){
        filename = filename.take(filename.lastIndexOf('.'))
    }
    return filename;
}

stage("Checkout"){
    node("master"){
        dir(PROJECT_NAME){
            checkout($class: 'GitSCM',
            branches: scm.branches,
            doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
            extensions: scm.extensions + [$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false],
            userRemoteConfigs: scm.userRemoteConfigs,
            quiet: true)
        }
        stash include: PROJECT_NAME, name: "source_code"
    }
}

def step_build_test = [:]
def step_test = [:]
def step_build_app = [:]
def step_archive = [:]

for(n in getLabelledNodes("MEDEA")){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME){
                dir("test/bin"){
                    //Clean the test directory
                    deleteDir()
                }
                dir("build"){
                    //Clean the build directory
                    deleteDir()
                    //Build the testing 
                    buildProject("Ninja", "-DBUILD_TEST=ON -DBUILD_APP=OFF -DBUILD_CLI=OFF")
                }
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

                def test_count = 0;
                def test_error_count = 0;

                //Find all executables
                def test_list = findFiles glob: globstr
                for (; test_count < test_list.size(); test_count++){
                    def file_path = test_list[test_count].name
                    def file_name = trimExtension(file_path)
                    def test_output = file_name + ".xml"
                    print("Running Test: " + file_path)
                    def test_error_code = runScript("./" + file_path + " -o " + test_output + ",lightxml")

                    if(test_error_code != 0){
                        test_error_count ++
                    }
                }

                //Archive the tests
                def test_archive = node_name + "_tests.zip"
                zip glob: "*.xml", zipFile: test_archive
                archiveArtifacts test_archive

                print("Node: " + node_name + " passed " + (test_count - test_error_count) + "/" + test_count + " tests.")

                if(test_count > 0){
                    if(test_error_count > 0){
                        currentBuild.result = 'Failure'
                    }
                }
            }
        }
    }

    step_build_app[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/build"){
                //Rebuild with everything
                buildProject("Ninja", "-DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_CLI=ON")
                runScript("cpack")
            }
        }
    }

    step_archive[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/build/installers"){
                def globstr = ""

                if(isUnix()){
                    globstr = '*.dmg'
                }else{
                    globstr = '*.exe'
                }
                def file_list = findFiles glob: globstr

                for(def file : file_list){
                    def archiveName = trimExtension(file.name) + "-installer.zip"
                    
                    zip glob: globstr, zipFile: archiveName
                }
                archiveArtifacts "*.zip"
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
    parallel step_build_app
}

stage("Archive"){
    parallel step_archive
}