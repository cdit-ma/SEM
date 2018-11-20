

node("docker_runtime") {
    checkout scm
    
    def minimal_deps = docker.build("minimal_deps", "-f docker/minimal_deps/Dockerfile .")
    def full_deps = docker.build("full_deps", "-f docker/full_deps/Dockerfile .")
    def re_minimal = docker.build("re_minimal", "-f docker/re_minimal/Dockerfile .")
    def re_full = docker.build("re_full", "-f docker/re_full/Dockerfile .")
}