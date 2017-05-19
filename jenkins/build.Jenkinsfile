@NonCPS
def nodeNames() {
  return jenkins.model.Jenkins.instance.nodes.collect { node -> node.name }
}

def getLabels(String name){
    def computer = Jenkins.getInstance().getComputer(name)
    def node = computer.getNode()
    if(computer.isOnline()){
        return node.getLabelString()
    }
    return ""
}

def names = nodeNames()
//filter nodes
def filtered_names = []
for(n in names){
    if(getLabels(n).contains("logan") && getLabels(n).contains("test_env")){
        filtered_names << n
        print("Got Node: " + n)
    }
}

if(filtered_names.empty()){
    print("###### Error: No valid nodes found ######")
}


stage('Checkout'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                dir('logan'){
                    deleteDir()
                    checkout([$class: 'GitSCM', branches: [[name: '*/master']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'WipeWorkspace'], [$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: false, recursiveSubmodules: true, reference: '', trackingSubmodules: false]], submoduleCfg: [], userRemoteConfigs: [[credentialsId: '1f1e1fcb-b84a-48a4-be37-0aac3118caa0', url: 'https://github.com/cdit-ma/logan']]])
                }
            }
        }
    }
    parallel builders
}

stage('Build'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                environment{
                    CMAKE_MODULE_PATH = pwd() + 'logan/cmake_modules'
                }
                dir('logan/build'){
                    sh 'cmake ..'
                    sh 'make -j6'
                }
            }
        }
    }
    parallel builders
}

stage('Test'){
    def builders = [:]
    for(n in filtered_names){
        def node_name = n
        builders[node_name] = {
            node(node_name){
                print('Running tests here')
            }
        }
    }
    parallel builders
}
