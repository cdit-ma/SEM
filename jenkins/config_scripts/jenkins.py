import subprocess
import urllib.request
import os


class JenkinsCLIError(Exception):
    def __init__(self, message):
        self.message = message


class JobConfig:

    def __init__(self, job_name: str, credential_id: str, file_path: str):
        self.job_name = job_name
        self.credential_id = credential_id
        self.file_path = file_path


class NodeConfig:
    """Node config struct.

    Used to parse jenkins node configuration from CSV document. Example format can be found in containing directory."""

    def __init__(self, node_name: str, ip_address: str, executor_count: int, credential_id: str, labels: str):
        self.node_name = node_name
        self.executor_count = executor_count
        self.ip_address = ip_address
        self.credential_id = credential_id
        self.labels = labels


class JenkinsHandle:
    """Jenkins handle class. Enables RAII style use of a Jenkins CLI interface.

    Automatically downloads a jenkins cli client and removes upon tidy up."""
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

    def create_job(self, job: JobConfig):
        print("Creating Jenkins job: " + job.job_name)
        create_job_command = ["java", "-jar", self.jenkins_jar_file_name, "-s", self.server_url, "create-job", job.job_name]
        print(create_job_command)
        with open(job.file_path) as job_descriptor_fh:
            job_descriptor = job_descriptor_fh.read()
            mutated_xml_string = job_descriptor.replace("##CREDENTIAL_ID##", job.credential_id)
            try:
                process = subprocess.Popen(create_job_command, stdin=subprocess.PIPE)
                process.communicate(mutated_xml_string.encode())
                process.wait()
                if process.returncode != 0:
                    raise JenkinsCLIError("Failed to create job: {0}".format(job.job_name))
            except:
                raise JenkinsCLIError("Failed to create job: {0}".format(job.job_name))

    def create_node(self, node_config: NodeConfig):
        print("Creating Jenkins node: " + node_config.node_name)

        with open("default_node_config.xml", 'r') as default_node_config_fh:
            # Replace instances of node attribute tags with attribute values.
            # This is algorithmically poor. Poor performance doesn't really matter given input size.
            xml_string = default_node_config_fh.read()
            mutated_xml_string = xml_string \
                .replace("##HOSTNAME##", node_config.node_name) \
                .replace("##NUM_EXECUTORS##", str(node_config.executor_count)) \
                .replace("##IP_ADDRESS##", node_config.ip_address) \
                .replace("##CREDENTIAL_ID##", node_config.credential_id) \
                .replace("##LABELS##", node_config.labels)

            create_node_command = ["java", "-jar", self.jenkins_jar_file_name, "-s", self.server_url, "create-node",
                                   node_config.node_name]
            print(create_node_command)
            try:
                process = subprocess.Popen(create_node_command, stdin=subprocess.PIPE)
                process.communicate(mutated_xml_string.encode())
                process.wait()
                if process.returncode != 0:
                    raise JenkinsCLIError("Failed to create node: {0}".format(node_config.node_name))
            except:
                raise JenkinsCLIError("Failed to create node: {0}".format(node_config.node_name))


