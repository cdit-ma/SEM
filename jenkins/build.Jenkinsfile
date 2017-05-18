
node('test_env'){
    stage('Checkout'){
        dir('logan'){
            deleteDir()
            checkout([$class: 'GitSCM', branches: [[name: '*/develop']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false]], submoduleCfg: [], userRemoteConfigs: [[credentialsId: "${GIT_CREDENTIAL_ID}", url: 'https://github.com/cdit-ma/logan.git']]])
        }
    }

    stage('Build'){
        environment{
            CMAKE_MODULE_PATH = pwd() + 'logan/cmake_modules'
        }
        dir('logan/build'){
            sh 'cmake ..'
            sh 'make -j6'
        }
    }
}
