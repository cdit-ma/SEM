/**
This script will generate a new rollout.zip by pulling from the CDIT-MA orginisation's github server.
This script should be used to generate a new rollout for deployment to airgapped networks.
The .zip generated by this job can be uploaded to the airgapped network by using the "upload_new_rollout" job.
This script requires the following Jenkins parameters:
- "GIT_CREDENTIALS_ID" : Username with password credential
- "GIT_URL" : String parameter (Defaulted to https://github.com/cdit-ma/SEM)
- "SEM_TAG" : String parameter. Version of SEM environment to checkout
- "SEM_BRANCH" : String parameter. Branch of SEM environment to checkout.
This script requires the following Jenkins plugins:
-Pipeline: Utility Steps
*/

@Library('cditma-utils')
import cditma.Utils
def utils = new Utils(this);


node("master"){
    echo sh(returnStdout: true, script: 'env')
    final GIT_BRANCH = env.JOB_BASE_NAME
    final CDITMA_GIT_URL = "https://github.com/cdit-ma/"
    final ROLLOUT_FILE_NAME = "re-" + GIT_BRANCH + "-rollout.tar.gz"

    //Set the Label
    currentBuild.description = GIT_BRANCH
    print("GIT_BRANCH: " + GIT_BRANCH)
    stage("Checkout"){
        checkout scm
    }
    stage("Archive"){
        files = []
        
        dir("re"){
            utils.runScript('git bundle create ../re.bundle ' + GIT_BRANCH)
            utils.runScript('git-archive-all ../re.tar.gz')
            files += 're.bundle'
            files += 're.tar.gz'
        }
        //Create archive
        utils.runScript('tar -czf ' + ROLLOUT_FILE_NAME + ' ' + files.join(' '))
        archiveArtifacts(ROLLOUT_FILE_NAME)
        deleteDir();
    }
}