def PROJECT_NAME = 'test_logan'
//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

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

def re_nodes = utils.getLabelledNodes("build_logan")
for(n in re_nodes){
    def node_name = n

    step_build_test[node_name] = {
        node(node_name){
            unstash "source_code"
            dir(PROJECT_NAME + "/build"){
                //Build the entire project 
                utils.buildProject("Unix Makefiles", "")
            }
        }
    }
}

stage("Build"){
    parallel step_build_test
}
