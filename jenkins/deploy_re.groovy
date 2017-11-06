/**
This script will deploy re to all jenkins slaves with the re label.
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

@Library('cdit-ma-utils')
import cdit-ma.*;

def PROJECT_NAME = 're'
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
