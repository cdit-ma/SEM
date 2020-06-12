import argparse
import csv
import jenkins

parser = argparse.ArgumentParser(description="Configure jenkins nodes required for system modelling.")
parser.add_argument("jenkins_server_ip_addr", action="store")
args = parser.parse_args()

server_address = args.jenkins_server_ip_addr
server_url = "http://" + server_address + ":8080/"

node_list_csv_file_name = "node_list.csv"

# Get node list
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

# Create jenkins node for each name in list.
# For each node there must be a csv entry of the following format:
# node_name,ip_address,executor_count,credential_id,labels
# Where labels are space separated
with jenkins.JenkinsHandle(server_url) as jenkins_handle:
    for node in node_list:
        try:
            jenkins_handle.create_node(node)
        except jenkins.JenkinsCLIError as err:
            print("Node creation error {0}".format(err))
        except:
            raise
