
// Override this to enable running of long tests
final Boolean IS_TAG = env.TAG_NAME
final GIT_ID = IS_TAG ? env.TAG_NAME : env.BRANCH_NAME

final minimal_deps_tag = "minimal_deps:${GIT_ID}"
final full_deps_tag = "full_deps:${GIT_ID}"
final re_minimal_tag = "re_minimal:${GIT_ID}"
final re_full_tag = "re_full:${GIT_ID}"

node("docker_runtime") {
    checkout scm

    docker.withRegistry('http://192.168.111.98:5000'){
        def minimal_deps = docker.build(minimal_deps_tag, "-f docker/minimal_deps/Dockerfile .")
        minimal_deps.push()

        def full_deps = docker.build(full_deps_tag, "-f docker/full_deps/Dockerfile .")
        full_deps.push()

        def re_minimal = docker.build(re_minimal_tag, "-f docker/re_minimal/Dockerfile .")
        re_minimal.push()
        sh "docker save ${re_minimal_tag} > ${re_minimal_tag}.tar"
        archiveArtifacts("${re_minimal_tag}.tar")

        def re_full = docker.build(re_full_tag, "-f docker/re_full/Dockerfile .")
        re_full.push()
        sh "docker save ${re_full_tag} > ${re_full_tag}.tar"
        archiveArtifacts("${re_full_tag}.tar")
    }
}