import argparse
import csv
import jenkins

parser = argparse.ArgumentParser(description="Configure jenkins jobs required for system modelling.")
parser.add_argument("jenkins_server_ip_addr", action="store")
args = parser.parse_args()

server_address = args.jenkins_server_ip_addr
server_url = "http://" + server_address + ":8080/"

job_list_csv_file_name = "job_list.csv"

# Get job list
job_list = []
with open(job_list_csv_file_name) as job_list_csv:
    csv_reader = csv.DictReader(job_list_csv, delimiter=',')
    for row in csv_reader:
        job_list.append(row["job_name"])

# Create jenkins job for each name in list.
# For each job name there must exist a directory of the same name containing a config.xml file.
with jenkins.JenkinsHandle(server_url) as jenkins_handle:
    for job in job_list:
        config_file_path = job + "/config.xml"
        jenkins_handle.create_job(job, config_file_path)
