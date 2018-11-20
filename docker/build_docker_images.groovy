
// Override this to enable running of long tests
final Boolean IS_TAG = env.TAG_NAME
final GIT_ID = IS_TAG ? env.TAG_NAME : env.BRANCH_NAME

node("docker_runtime") {
    checkout scm
    
    docker.withRegistry('http://192.168.111.98:5000'){
        def minimal_deps = docker.build("minimal_deps:${GIT_ID}", "-f docker/minimal_deps/Dockerfile .")
        minimal_deps.push()
        def full_deps = docker.build("full_deps:${GIT_ID}", "-f docker/full_deps/Dockerfile .")
        full_deps.push()
        def re_minimal = docker.build("re_minimal:${GIT_ID}", "-f docker/re_minimal/Dockerfile .")
        re_minimal.push()
        def re_full = docker.build("re_full:${GIT_ID}", "-f docker/re_full/Dockerfile .")
        re_full.push()
    }
}