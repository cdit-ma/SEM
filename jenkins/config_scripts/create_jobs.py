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
        job_config = jenkins.JobConfig(job_name=row["job_name"],
                                       credential_id=row["credential_id"])
        job_list.append(job_config)

# Create jenkins job for each name in list.
# For each job name there must exist a directory of the same name containing a config.xml file.
with jenkins.JenkinsHandle(server_url) as jenkins_handle:
    for job in job_list:
        try:
            jenkins_handle.create_job(job)
        except jenkins.JenkinsCLIError as err:
            print("Job creation error {0}".format(err))
        except:
            raise
