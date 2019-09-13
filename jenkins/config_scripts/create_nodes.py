import argparse
import csv
import jenkins

parser = argparse.ArgumentParser(description="Configure jenkins jobs required for system modelling.")
parser.add_argument("jenkins_server_ip_addr", action="store")
args = parser.parse_args()

server_address = args.jenkins_server_ip_addr
server_url = "http://" + server_address + ":8080/"

node_list_csv_file_name = "node_list.csv"

# Get job list
node_list = []
with open(node_list_csv_file_name) as node_list_csv:
    csv_reader = csv.DictReader(node_list_csv, delimiter=',')
    for row in csv_reader:
        node_config = jenkins.NodeConfig(node_name=row["node_name"],
                                         ip_address=row["ip_address"],
                                         executor_count=row["executor_count"],
                                         credential_id=row["credential_id"],
                                         labels=row["labels"])
        node_list.append(node_config)

# Create jenkins job for each name in list.
# For each job name there must exist a directory of the same name containing a config.xml file.
with jenkins.JenkinsHandle(server_url) as jenkins_handle:
    for node in node_list:
        jenkins_handle.create_node(node)
