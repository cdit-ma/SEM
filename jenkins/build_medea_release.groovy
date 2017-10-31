/**
This script will build MEDEA in release mode and archive all required build artefacts.
This script requires the following Jenkins parameters:
- "GIT_CREDENTIAL_ID" : Username with password credential
- "GIT_URL" : String parameter (Defaulted to https://github.com/cdit-ma/medea)
- "GIT_BRANCH" : Optional branch to checkout, overridden by GIT_TAG
- "GIT_TAG" : Optional version tag, ignores any branch specified
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

def PROJECT_NAME = 'medea'

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

//Run script, changes to bat if windows detected.
def runScript(String script){
    if(isUnix()){
        out = sh(returnStatus: true, script: script)
        return out
    }
    else{
        out = bat(returnStatus:true, script: script)
        return out
    }
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

node("MEDEA"){
    deleteDir()

    if(!checkParamsExist()){
        currentBuild.result = 'Failure'
        return
    }

    git_credential_id = getCredentialID()
    git_url = buildGitUrl(env.GIT_URL, PROJECT_NAME)
    ref_name = buildGitRef(env.GIT_BRANCH, env.GIT_TAG)
    currentBuild.description = git_url + '/' + ref_name


    stage("Checkout"){
        dir('medea'){
            checkout([$class: 'GitSCM', branches: [[name: ref_name]], doGenerateSubmoduleConfigurations: false,
            extensions: [[$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false],
                         [$class: 'CloneOption', depth: 0, noTags: false, reference: '', shallow: true]],
            submoduleCfg: [], userRemoteConfigs: [[credentialsId: git_credential_id, url: git_url]]])
        }
    }

    stage("Build"){
        dir('medea'){
            dir('build'){
                print "Calling CMake generate"
                runScript("cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release")
                print "Finished Generating Makefiles"
                print "Calling CMake --build"
                runScript("cmake --build . --config Release")
                print "Finished Build"
            }
        }
    }

    stage("Pack"){
        print "Running CPack"
        dir('medea'){
            dir('build'){
                runScript("cpack")
            }
        }
        print "Finished running CPack"
    }

    stage("Archive"){


        dir("build"){
            dir("installers"){

                def globstr = ""

                if(isUnix()){
                    globstr = '*.dmg'
                }else{
                    globstr = '*.exe'
                }
                def fileList = findFiles glob: globstr

                archiveName = fileList[0].name.substring(0, fileList[0].name.length() - 4) + "-installer"

                if(env.GIT_TAG){
                    archiveName = archiveName + "-" + env.GIT_TAG
                }else{
                    archiveName = archiveName + "-" + env.GIT_BRANCH
                }
                archiveName = archiveName + ".zip"



                zip glob: globstr, zipFile: archiveName
                archiveArtifacts "*.zip"
            }
        }
    }
}
