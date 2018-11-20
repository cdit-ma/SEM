

node("docker_runtime") {
    checkout scm
    def minimal_deps = docker.build("minimal_deps", "docker/minimal_deps")
    def full_deps = docker.build("full_deps", "docker/full_deps")
    def re_minimal = docker.build("re_minimal", "docker/re_minimal")
    def re_full = docker.build("re_full", "docker/re_full")
}