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
    if(runScript("cmake .. -G " + generator + " -DCMAKE_BUILD_TYPE=Release " + cmake_options) == 0){
        print "Calling CMake --build"
        if(runScript("cmake --build . --config Release") == 0){
            return true;
        }
    }
    currentBuild.result = 'Failure'
    return
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
        stash includes: "**", name: "source_code"
    }
}

def step_build_test = [:]
def step_test = [:]
def step_archive = [:]

def medea_nodes = getLabelledNodes("MEDEA")
for(n in medea_nodes){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //Build the entire project 
                buildProject("Ninja", "-DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_CLI=ON")
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
                runScript("cpack")
                dir("installers"){
                    def globstr = ""

                    if(isUnix()){
                        globstr = '*.dmg'
                    }else{
                        globstr = '*.exe'
                    }

                    def file_list = findFiles glob: globstr

                    def archiveName = trimExtension(file_list[0].name) + "-installer.zip"
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