/**
This script will deploy re to all jenkins slaves with the re label.
This script requires the following Jenkins parameters:
- "GIT_CREDENTIAL_ID" : Username with password credential
- "GIT_URL" : String parameter (Defaulted to https://github.com/cdit-ma/re)
- "GIT_BRANCH" : Optional branch to checkout, overridden by GIT_TAG
- "GIT_TAG" : Optional version tag, ignores any branch specified
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

def PROJECT_NAME = 're'

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



def git_url = "/srv/git"

def re_nodes = getLabelledNodes(PROJECT_NAME);

//Checkout and stash re source
stage('Checkout'){
    node('master'){
        dir(git_url + "/" + PROJECT_NAME){
            stash include: "**", name: "source_code"
        }
    }
}

print("TOTOTLTLTL")
def step_build = [:]

for(n in re_nodes){
    def node_name = n
    step_build[node_name] = {
        node(node_name){
            dir("${RE_PATH}"){
                deleteDir()
                unstash "source_code"
                dir("build"){
                    //Build the entire project 
                    buildProject("Unix Makefiles", "")
                }
            }
        }
    }
}
//Build re on all re nodes
stage('Build'){
    parallel step_build
}
