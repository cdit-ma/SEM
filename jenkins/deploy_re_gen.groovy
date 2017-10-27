/**
This script will deploy re_gen to all jenkins slaves with the re label.
This script requires the following Jenkins parameters:
- "GIT_CREDENTIAL_ID" : Username with password credential
- "GIT_URL" : String parameter (Defaulted to https://github.com/cdit-ma/re_gen)
- "GIT_BRANCH" : Optional branch to checkout, overridden by GIT_TAG
- "GIT_TAG" : Optional version tag, ignores any branch specified
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

def PROJECT_NAME = 're_gen'

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

//Check all required params
def checkParamsExist(){
    def git_url = ""
    git_url = env.GIT_URL
    if(git_url == null){
        print("ERROR: \"GIT_URL\" string parameter missing in build configuration")
        return false
    }
    def git_credential_id = ""
    git_credential_id = env.GIT_CREDENTIAL_ID
    if(git_credential_id == null) {
        print("ERROR: \"GIT_CREDENTIAL_ID\" credential parameter missing in build configuration")
        return false
    }
    def branch = env.GIT_BRANCH
    if(branch == null){
        print("ERROR: \"GIT_BRANCH\" string parameter missing in build configuration")
        return false
    }

    def tag = env.GIT_TAG
    if(tag == null){
        print("ERROR: \"GIT_TAG\" string parameter missing in build configuration")
        return false
    }

    return true
}

def getCredentialID(){
    return env.GIT_CREDENTIAL_ID
}

//Build git ref pointer. If no branch specified, use tag, if no tag specified use default branch (master).
def buildGitRef(String branch, String tag){
    def default_branch = "master"
    def ref_name = ""

    if(!branch){
        branch = default_branch
    }
    ref_name = "*/" + branch

    if(tag){
        ref_name = "refs/tags/" + tag
    }
    return ref_name
}

//Build git url. If no url specified, default to cdit-ma github+proj name
def buildGitUrl(String url, String proj){
    def default_git_url = "https://github.com/cdit-ma/" + proj + ".git"
    //Set git url to default if empty
    if(!url){
        return default_git_url
    }
    return url
}


def ref_name = ""
def git_url = ""
def git_credential_id = ""
def filtered_names = []

stage('Set up'){
    //Start deploy script
    if(!checkParamsExist()){
        currentBuild.result = 'FAILURE'
        return
    }

    //Get nodes to deploy to
    def names = nodeNames()
    for(n in names){
        if(getLabels(n).contains("re")){
            filtered_names << n
            print("Got Node: " + n)
        }
    }

    //Build git url and ref
    git_credential_id = getCredentialID()
    git_url = buildGitUrl(env.GIT_URL, PROJECT_NAME)
    ref_name = buildGitRef(env.GIT_BRANCH, env.GIT_TAG)
    currentBuild.description = git_url + '/' + ref_name
}


//Checkout re_gen source onto master node and stash
stage('Checkout'){
    node("master"){
        dir("re_gen"){
            deleteDir()
            checkout([$class: 'GitSCM', branches: [[name: ref_name]], doGenerateSubmoduleConfigurations: false,
                extensions: [[$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false]],
                submoduleCfg: [], userRemoteConfigs: [[credentialsId: git_credential_id, url: git_url]]])
            stash includes: "**", name: "re_gen_source"
        }
    }
}

//Unstash and build on all re nodes
stage('Deploy'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                dir("${RE_GEN_PATH}"){
                    unstash "re_gen_source"
                }
            }
        }
    }
    parallel builders
}


