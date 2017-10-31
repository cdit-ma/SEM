def PROJECT_NAME = 'test_medea'

//Run script, changes to bat if windows detected.
def runScript(String script){
    if(isUnix()){
        out = sh(returnStatus: true, script: script)
        return out
    }
    else{
        out = bat(returnStatus:true, script: script)
        return out
    }
}

node("MEDEA"){
    dir(PROJECT_NAME){
        stage("Checkout"){
            checkout git
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
            def globstr = "test/bin/*"
            def test_list = findFiles glob: globstr

            for(test in test_list){
                print("Running Test: " + test)
                runScript(test)
            }
        }
    }
}