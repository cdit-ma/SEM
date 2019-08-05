/**
This script will deploy re_gen to all jenkins slaves with the re label.
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

def PROJECT_NAME = 're_gen'
def git_url = "/srv/git"
def re_gen_nodes = utils.getLabelledNodes("re");

//Checkout and stash re_gen source (stored on Master's local git repo)
stage('Checkout'){
    node('master'){
        dir(git_url + "/" + PROJECT_NAME){
            stash include: "**", name: "source_code"
        }
    }
}

//Construct build map for all nodes which should build
def step_build = [:]
for(n in re_gen_nodes){
    def node_name = n
    step_build[node_name] = {
        node(node_name){
            dir("${RE_GEN_PATH}"){
                deleteDir()
                unstash "source_code"
            }
        }
    }
}

//Build logan on all logan nodes
stage('Build'){
    parallel step_build
}
