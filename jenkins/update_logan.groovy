
def git_path = "/srv/git/logan";

//Pull from the setup SCM in Jenkins Job
stage("checkout") {
    node("master"){
        dir(git_path){
            deleteDir();
            checkout(scm);
        }
    }
}

currentBuild.description = env.BRANCH

//Deploy and build
build(job: 'deploy_logan', quietPeriod: 0);