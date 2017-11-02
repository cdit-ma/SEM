def PROJECT_NAME = 'test_re'

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
    if(runScript("cmake .. -G \"" + generator + "\" -DCMAKE_BUILD_TYPE=Release " + cmake_options) == 0){
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
            checkout scm
        }
        stash includes: "**", name: "source_code"
    }
}

def step_build_test = [:]
def step_test = [:]

def re_nodes = getLabelledNodes("build_re")
for(n in re_nodes){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //Build the entire project 
                buildProject("Unix Makefiles", "-DBUILD_TEST=ON")
            }
        }
    }

    step_test[node_name] = {
        node(node_name){
            dir(PROJECT_NAME + "/bin/test"){
                def globstr = "*"

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
                        def test_error_code = runScript("../" + file_path + " --gtest_output=xml:" + test_output)

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
}

stage("Build"){
    parallel step_build_test
}
stage("Test"){
    parallel step_test
}

node("master"){
    dir("test_cases"){
        for(n in re_nodes){
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