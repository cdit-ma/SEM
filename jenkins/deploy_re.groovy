/**
This script will deploy re to all jenkins slaves with the re label.
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

//Load shared pipeline utility library
@Library('cditma-utils')
import cditma.Utils

def utils = new Utils(this);

final PROJECT_NAME = 're'
def git_url = "/srv/git"
def re_nodes = utils.getLabelledNodes(PROJECT_NAME);


final ARCHIVE_NAME = PROJECT_NAME +".tar.gz"
final STASH_NAME = ARCHIVE_NAME + "_stash"

//Checkout and stash re source archive (stored on Master's local git repo)
stage('Checkout'){
    node('master'){
        dir(git_url){
            stash includes: ARCHIVE_NAME, name: STASH_NAME
        }
    }
}

//Construct build map for all nodes which should build
def step_build = [:]
for(n in re_nodes){
    def node_name = n
    step_build[node_name] = {
        node(node_name){
            dir("${HOME}/re"){
                deleteDir()
            }
            unstash STASH_NAME

            //Build into HOME
            utils.runScript("tar -xf " + ARCHIVE_NAME + " -C ${HOME}" )

            dir("${HOME}/re/build"){
                touch '.dummy'
                print("Running in: " + pwd())
                def result = utils.buildProject("Ninja", "")
                if(!result){
                    error('Failed to compile')
                }
            }
        }
    }
}

//Build re on all re nodes
stage('Build'){
    parallel step_build
}
