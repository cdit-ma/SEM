/**
This script will deploy cmake_modules to all jenkins slaves with the cmake_modules label.
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

def PROJECT_NAME = 'cmake_modules'
def git_url = "/srv/git"
def cmake_modules_nodes = utils.getLabelledNodes("re");

//Checkout and stash cmake_modules source (stored on Master's local git repo)
stage('Checkout'){
    node('master'){
        dir(git_url + "/" + PROJECT_NAME){
            stash include: "**", name: "source_code"
        }
    }
}

//Construct build map for all nodes which should build
def step_build = [:]
for(n in cmake_modules_nodes){
    def node_name = n
    step_build[node_name] = {
        node(node_name){
            dir("${CMAKE_MODULE_PATH}"){
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
