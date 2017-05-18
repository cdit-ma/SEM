
node('test_env'){
    stage('Checkout'){
        dir('logan'){
            deleteDir()
            checkout([$class: 'GitSCM', branches: [[name: '*/develop']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: false, reference: '', trackingSubmodules: false]], submoduleCfg: [], userRemoteConfigs: [[url: 'https://github.com/cdit-ma/logan.git']]])
        }
    }

    stage('Build'){
        dir('logan/build'){
            sh 'cmake ..'
            sh 'make -j6'
        }
    }

}

