def git_path = "/srv/git/re";
stage("checkout") {
    node("master"){
        dir(git_path){
            deleteDir();
            checkout(scm);
        }
    }
}

build job: 'deploy_re'