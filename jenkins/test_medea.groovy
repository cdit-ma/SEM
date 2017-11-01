def PROJECT_NAME = 'test_medea'

//Run script, changes to bat if windows detected.
def runScript(String script){
    if(isUnix()){
        out = sh(returnStatus: true, script: script)
        return out
    }
    else{
        out = powershell(returnStatus:true, script: script)
        return out
    }
}

node("MEDEA"){
    dir(PROJECT_NAME){
        stage("Checkout"){
            checkout scm
        }

        //cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -DBUILD_APP=OFF -DBUILD_CLI=OFF"
        stage("Build"){
            dir("build"){
                print "Calling CMake generate"
                def build_options = "-DBUILD_TEST=ON -DBUILD_APP=OFF -DBUILD_CLI=OFF"
                runScript("cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release " + build_options)
                print "Calling CMake --build"
                runScript("cmake --build . --config Release")
                print "Finished Build"
            }
        }
        
        def test_count = 0
        def test_error_count = 0
        stage("Test"){
            dir("test/bin"){
                def globstr = "*"
                if(!isUnix()){
                    globstr = '*.exe'
                }

                //Find all executables
                def test_list = findFiles glob: globstr
                for (int i = 0; i < test_list.size(); i++){
                    test_count ++
                    def file_path = test_list[i].name
                    print("Running Test: " + file_path)
                    def test_error_code = runScript("./" + file_path)

                    if(test_error_code != 0)
                        test_error_count ++
                    }
                }
            }
        }

        currentBuild.description = 'Passed ' + (test_count - test_error_count)+ '/' + test_count + ' Test Cases'
        if(test_count > 0){
            if(test_error_count > 0){
                currentBuild.result = 'Unstable'
            }else{
                currentBuild.result = 'Success'
            }
        }else{
            currentBuild.result = 'Failure'
        }

        
    }
}