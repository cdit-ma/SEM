/**
This script will deploy logan to all jenkins slaves with the logan label.
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

def PROJECT_NAME = 'logan'
def git_url = "/srv/git"
def logan_nodes = utils.getLabelledNodes(PROJECT_NAME);

//Checkout and stash logan source (stored on Master's local git repo)
stage('Checkout'){
    node('master'){
        dir(git_url + "/" + PROJECT_NAME){
            stash include: "**", name: "source_code"
        }
    }
}

//Construct build map for all nodes which should build
def step_build = [:]
for(n in logan_nodes){
    def node_name = n
    step_build[node_name] = {
        node(node_name){
            dir("${LOGAN_PATH}"){
                deleteDir()
                unstash "source_code"
                dir("build"){
                    def result = utils.buildProject("Ninja", "")
                    if(!result){
                        error('Failed to compile')
                    }
                }
            }
        }
    }
}

//Build logan on all logan nodes
stage('Build'){
    parallel step_build
}