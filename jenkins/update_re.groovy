def git_path = "/srv/git/re";
stage("checkout") {
    node("master"){
        sh 'rm -rf ' + git_path + '/*'
        sh 'mv * ' + git_path
        sh 'ls ' + git_path
        deleteDir();
    }
}