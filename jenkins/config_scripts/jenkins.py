import subprocess
import urllib.request
import os

from typing import List


class JenkinsCLIError(Exception):
    def __init__(self, message):
        self.message = message


class NodeConfig:
    def __init__(self, node_name: str, ip_address: str, executor_count: int, credential_id: str, labels: str):
        self.node_name = node_name
        self.executor_count = executor_count
        self.ip_address = ip_address
        self.credential_id = credential_id
        self.labels = labels


class JenkinsHandle:
    def __init__(self, server_url: str):
        self.jenkins_jar_file_name = "jenkins-cli.jar"
        self.server_url = server_url

    def __enter__(self):
        jar_url = self.server_url + "/jnlpJars/jenkins-cli.jar"
        try:
            with urllib.request.urlopen(jar_url) as response, open(self.jenkins_jar_file_name, 'wb') as out_file:
                data = response.read()
                out_file.write(data)
        except:
            raise JenkinsCLIError("Failed to download Jenkins cli jar")

        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        try:
            os.remove(self.jenkins_jar_file_name)
        except:
            pass

    def create_job(self, job_name: str, file_path: str):
        print("Creating Jenkins job: " + job_name)
        # TODO: Replace print with subprocess call
        create_job_command = ["java", "-jar", self.jenkins_jar_file_name, "-s", self.server_url, "create-job", job_name]
        print(create_job_command)
        # process = subprocess.Popen(create_job_command, stdin=subprocess.PIPE)
        # process.communicate(input=open(file_path).read().encode())

    def create_node(self, node_config: NodeConfig):
        print("Creating Jenkins node: " + node_config.node_name)

        # XXX: THIS IS AN UGLY HACK
        with open("default_node_config.xml", 'r') as default_node_config_fh:
            xml_string = default_node_config_fh.read()
            mutated_xml_string = xml_string \
                .replace("##HOSTNAME##", node_config.node_name) \
                .replace("##NUM_EXECUTORS##", str(node_config.executor_count)) \
                .replace("##IP_ADDRESS##", node_config.ip_address) \
                .replace("##CREDENTIAL_ID##", node_config.credential_id) \
                .replace("##LABELS##", node_config.labels)

            # TODO: Replace print with subprocess call
            print(mutated_xml_string)

        # TODO: Pipe mutated xml file into create_node_command run
        create_node_command = ["java", "-jar", self.jenkins_jar_file_name, "-s", self.server_url, "create-node",
                               node_config.node_name]
        print(create_node_command)
        # process = subprocess.Popen(create_node_command, stdin=subprocess.PIPE)
        # process.communicate(input=mutated_xml_string)
