#!groovy
@Library('cditma-utils') _
def utils = new cditma.Utils(this);


final workspace_dir = env.BUILD_ID
def model_dir = ""

pipeline{
    agent{node 'master'}

    parameters{
        string(name: 'experiment_prefix', defaultValue: '', description: 'The prefix to be given to each experiment')
        string(name: 'execution_time', defaultValue: '60', description: 'The duration to run each experiment for')
        string(name: 'environment_manager_address', defaultValue: "${env.ENVIRONMENT_MANAGER_ADDRESS}", description: 'The address of the Environment Manager to use for this experiment')
        string(name: 'log_verbosity', defaultValue: '3', description: 'The logging verbosity (1-10)')
        file(name: 'archive', description: 'The archive of models (.zip/.tar.gz/.tar.bz)')
    }

    stages{
        stage("Extracting Experiments"){
            steps{
                dir(workspace_dir){
                    script{
                        touch(".dummy")
                        //Unstashing files must be done relative to the workspace dir
                        unstashParam 'archive', "${workspace_dir}/models.archive"
                        final archive_name = "${archive}"

                        def extract_command = ""
                        if(archive_name.endsWith(".zip")){
                            extract_command = "unzip"
                        }else if(archive_name.endsWith(".tar.gz")){
                            extract_command = "tar xzf"
                        }else if(archive_name.endsWith(".tar.bz2")){
                            extract_command = "tar xjf"
                        }

                        if(!extract_command){
                            error("Unknown archive type: '${archive_name}'")
                        }

                        //Extract archive
                        if(utils.runScript(extract_command + " models.archive") != 0){
                            error('Extract archive failed')
                        }

                        model_dir = pwd()
                    }
                }
            }
        }
        stage("Execute Experiments"){
            steps{
                dir(model_dir){
                    script{
                        def execution_map = [:]
                        def model_files = findFiles(glob: '**/*.graphml')
                        def success_count = 0
                        def experiment_count = model_files.length
                        currentBuild.displayName = "#${env.BUILD_ID} - ${params.experiment_prefix} [${experiment_count}]"

                        def index = 0

                        for(def file : model_files){
                            def i = index++;
                            def file_path = file.name
                            def file_name = utils.trimExtension(file_path)
                            def experiment_name = "${params.experiment_prefix}-${i}-${file_name}"

                            execution_map[experiment_name] = {
                                //For some reason files can only be constructed from the Master Node and relative to WORKSPACE
                                def model = new File("${model_dir}/${file_path}")
                                def result = build(job: 'deploy_model', quietPeriod: 0, wait: true, propagate: false,
                                    parameters: [
                                        string(name: 'environment_manager_address', value: params.environment_manager_address),
                                        string(name: 'experiment_name', value: "${experiment_name}"),
                                        string(name: 'execution_time', value: params.execution_time),
                                        string(name: 'log_verbosity', value: params.log_verbosity),
                                        new FileParameterValue("model", model, file_path)
                                    ])
                                    
                                def downstream_build_id = "" + result.getNumber()
                                copyArtifacts(projectName: 'deploy_model', selector: specific(downstream_build_id), target: "artifacts/${downstream_build_id}")
                                
                                if(result.getResult() == "SUCCESS"){
                                    success_count ++;
                                }else{
                                    print("Experiment #${index} with file: '${file_path}}' Failed!")
                                }
                            }
                        }

                        parallel(execution_map)

                        dir("${model_dir}/artifacts"){
                            archiveArtifacts artifacts: '**', allowEmptyArchive: true
                        }
                        deleteDir()
                    }
                }
            }
        }
    }
}