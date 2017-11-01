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

def buildProject(String path, String generator, String cmake_options){
    dir(path){
        print "Calling CMake generate"
        runScript("cmake .. -G " + generator + " -DCMAKE_BUILD_TYPE=Release " + cmake_options)
        print "Calling CMake --build"
        runScript("cmake --build . --config Release")
    }
}

stage("Checkout"){
    node("master"){
        dir(PROJECT_NAME){
            checkout($class: 'GitSCM',
            branches: scm.branches,
            doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
            extensions: scm.extensions + [$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false],
            userRemoteConfigs: scm.userRemoteConfigs)
        }
        stash include: PROJECT_NAME, name: "source_code"
    }
}


def builders = [:]
for(n in getLabelledNodes("MEDEA")){
    def node_name = n
    builders[node_name] = {
        node(node_name){
            deleteDir()
            unstash "source_code"
            dir(PROJECT_NAME){
                stage("Build"){
                    //Build the testing
                    buildProject("build", "Ninja", "-DBUILD_TEST=ON -DBUILD_APP=OFF -DBUILD_CLI=OFF")
                }
                
                stage("Test"){
                    dir("test/bin"){
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
                            print("Running Test: " + file_path)
                            def test_error_code = runScript("./" + file_path)

                            if(test_error_code != 0){
                                test_error_count ++
                            }
                        }

                        currentBuild.description += node_name + ':Passed ' + (test_count - test_error_count)+ '/' + test_count + ' Test Cases'
                    }
                }
                stage("Pack"){
                    dir("build"){
                        //Run CPack
                        runScript("cpack")
                    }
                }

                stage("Archive"){
                    dir("build/installers"){
                        def globstr = ""

                        if(isUnix()){
                            globstr = '*.dmg'
                        }else{
                            globstr = '*.exe'
                        }
                        def fileList = findFiles glob: globstr

                        archiveName = fileList[0].name.substring(0, fileList[0].name.length() - 4) + "-installer"
                        archiveName = archiveName + ".zip"

                        zip glob: globstr, zipFile: archiveName
                        archiveArtifacts "*.zip"
                    }
                }
            }
        }
    }
}
parallel builders