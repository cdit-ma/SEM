def git_path = "/srv/git/re";
stage("checkout") {
    node("master"){
        sh 'rm -rf ' + git_path + '/*'
        dir(git_path){
            checkout scm
        }
        deleteDir();
    }
}