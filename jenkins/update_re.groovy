def git_path = "/srv/git/re";

//Pull from the setup SCM in Jenkins Job
stage("checkout") {
    node("master"){
        dir(git_path){
            deleteDir();
            checkout(scm);
        }
    }
}

currentBuild.description = evn.BRANCH

//Deploy and build
build(job: 'deploy_re', quietPeriod: 0);