/* This Jenkinsfile:
    1. Pulls the latest cmake_modules source from a specified git repo (defaults to github.com)
    2. Stashes+pops this repo to all nodes with the "re" tag
*/

//Collect a list of Node names from the current Jenkins instance
@NonCPS
def nodeNames() {
  return jenkins.model.Jenkins.instance.nodes.collect { node -> node.name }
}

//Get labels attached to node name
def getLabels(String name){
    def computer = Jenkins.getInstance().getComputer(name)
    def node = computer.getNode()
    if(computer.isOnline()){
        return node.getLabelString()
    }
    return ""
}

//Check that the correct params are specified in the jenkins configuration
def git_url = ""
git_url = env.GIT_URL
if(git_url == null){
    print("ERROR: \"GIT_URL\" string parameter missing in build configuration")
    currentBuild.result = 'FAILURE'
    return
}

def git_credential_id = ""
git_credential_id = env.GIT_CREDENTIAL_ID
if(git_credential_id == null){
    print("ERROR: \"GIT_CREDENTIAL_ID\" credential parameter missing in build configuration")
    currentBuild.result = 'FAILURE'
    return
}

def names = nodeNames()

//Filter out non logan nodes
def filtered_names = []
for(n in names){
    if(getLabels(n).contains("re")){
        filtered_names << n
        print("Got Node: " + n)
    }
}

//If param is empty, use default
def default_git_url = "https://github.com/cdit-ma/cmake_modules.git"
if(!git_url){
    git_url = default_git_url
}

//Checkout logan on master node and stash
stage('Checkout'){
    node("master"){
        dir("cmake_modules"){
            deleteDir()
            checkout([$class: 'GitSCM', branches: [[name: '*/master']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false]], submoduleCfg: [], userRemoteConfigs: [[credentialsId: git_credential_id, url: git_url]]])
            stash includes: "**", name: "cmake_modules_source"
        }
    }
}

//Unstash on all logan nodes
stage('Checkout'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                dir("cmake_modules"){
                    unstash "cmake_modules_source"
                }
            }
        }
    }
    parallel builders
}
