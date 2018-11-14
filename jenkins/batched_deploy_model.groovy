//This script requires the following Jenkins plugins:
// -Pipeline: Utility Steps
// -CopyArtifacts

//Requires following parameters in jenkins job:
// -String parameter: MODELS_ARCHIVE
// -String parameter: EXECUTION_TIME
// -String parameter: EXPERIMENT_PREFIX
// -String parameter: LOG_VERBOSITY


@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);

node("master"){
    final CLEANUP = true
    final ARCHIVE = 'models.archive'
    final DOWNSTREAM_JOB_NAME = 'deploy_model'
    final JOB_NAME = "${JOB_NAME}"
    final ORIGINAL_FILE = "${MODELS_ARCHIVE}"
    final EXPERIMENT_TIME = "${EXECUTION_TIME}"
    final EXPERIMENT_PREFIX = "${EXPERIMENT_PREFIX}"
    final LOG_VERBOSITY = "${LOG_VERBOSITY}"
    final build_id = env.BUILD_ID
    final workspace_dir = build_id + "-models"

    stage('Extracting Experiments'){
        //Unstash the archive
        unstashParam "MODELS_ARCHIVE", ARCHIVE

        dir(workspace_dir){
        
            def extract_command = ""

            if(ORIGINAL_FILE.endsWith(".zip")){
                extract_command = "unzip"
            }else if(ORIGINAL_FILE.endsWith(".tar.gz")){
                extract_command = "tar xzf"
            }else if(ORIGINAL_FILE.endsWith(".tar.bz2")){
                extract_command = "tar xjf"
            }

            if(!extract_command){
                error("Unknown archive type: '" + ORIGINAL_FILE + "'")
            }

            //Extract archive
            if(utils.runScript(extract_command + " ../" + ARCHIVE) != 0){
                error('Extract archive failed')
            }
        }
    }

    stage("Execute Experiments"){
        dir(workspace_dir){
            def model_files = findFiles(glob: '**/*.graphml')
            def success_count = 0
            def experiment_count = model_files.length

            currentBuild.displayName = "#" + build_id + " - " + EXPERIMENT_PREFIX + " [" + experiment_count + "]"

            def index = 0
            for(def file : model_files){
                def i = index++;
                def file_path = file.name
                def file_name = utils.trimExtension(file_path)
                def experiment_name = EXPERIMENT_PREFIX + "-" + i + "-" + file_name
                //For some reason files can only be constructed from the Master Node and relative to WORKSPACE
                def model = new File("${WORKSPACE}/" + workspace_dir + "/" + file_path)
                
                def result = build(job: DOWNSTREAM_JOB_NAME, quietPeriod: 0, wait: true, propagate: false,
                    parameters: [
                        string(name: 'EXECUTION_TIME', value: EXPERIMENT_TIME),
                        string(name: 'EXPERIMENT_NAME', value: experiment_name),
                        string(name: 'LOG_VERBOSITY', value: LOG_VERBOSITY),
                        new FileParameterValue("model", model, file_path)
                    ])
                
                def downstream_build_id = "" + result.getNumber()
                copyArtifacts(projectName: DOWNSTREAM_JOB_NAME, selector: specific(downstream_build_id), target: 'artifacts/' + downstream_build_id)
                
                if(result.getResult() == "SUCCESS"){
                    success_count ++;
                }else{
                    print("Experiment #" + index + " with file: ''" + file_path + "' Failed!'")
                }
            }

            dir('artifacts'){
                archiveArtifacts artifacts: '**/*'
            }

            //Delete the Dir
            if(CLEANUP){
                deleteDir()
            }
        }
    }
}