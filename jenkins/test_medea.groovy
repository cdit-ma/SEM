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

node("MEDEA && Windows10"){
    dir(PROJECT_NAME){
        stage("Checkout"){
            checkout scm
        }

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

                for(test in test_list){
                    def file_path = test.name
                    print("Running Test: " + file_path)
                    runScript("" + file_path)
                }
            }
        }
    }
}