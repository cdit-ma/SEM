def PROJECT_NAME = 'test_medea'

//Run script, changes to bat if windows detected.
def runScript(String script){
    print(script)
    if(isUnix()){
        out = sh(returnStatus: true, script: script)
        return out
    }
    else{
        out = bat(returnStatus:true, script: script)
        return out
    }
}

node("MEDEA && macOS"){
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
        
        stage("Test"){
            dir("test/bin"){
                def globstr = "*"
                def test_list = findFiles glob: globstr


                for (int i = 0; i < test_list.size(); i++){
                    def file_path = test_list[i].name
                    print("Running Test: " + file_path)
                    runScript("" + file_path)
                }
            }
        }
    }
}