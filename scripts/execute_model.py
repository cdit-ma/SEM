#!/usr/bin/python3
import os
import subprocess
import json
import signal
import sys
import argparse
import time
import threading

reThread = 0
clientThread = 0
loggerThread = 0


reProcess = 0
clientProcess = 0
serverProcess = 0

serverCondition = 0
clientCondition = 0
reCondition = 0

terminate = False
args = ""
current_dir = "."

interrupted = False

def signal_handler(signal, frame):
    interrupted = True

    reProcess.terminate()
    print("waiting for re")
    reProcess.wait()

    clientProcess.terminate()
    print("waiting for client")
    clientProcess.wait()

    serverProcess.terminate()
    print("waiting for server")
    serverProcess.wait()

    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

parser = argparse.ArgumentParser(description="Execute model locally")
parser.add_argument('modelFile', help="Model file path")
parser.add_argument('codeDir', help="Compiled model code dir")
parser.add_argument('executionTime', help="Time to execute model for")
args = parser.parse_args()

modelFile = args.modelFile
codeDir = args.codeDir
executionTime = args.executionTime

reGenPath = os.environ['RE_GEN_PATH']
saxonPath = reGenPath
rePath = os.environ['RE_PATH']
loganPath = os.environ['LOGAN_PATH']
cmakeModulePath = os.environ['CMAKE_MODULE_PATH']
qpidPath = os.environ['QPID_ROOT']
pugiPath = os.environ['PUGIXML_PATH']


middlewareString = ' middlewares='
fileString = '-s:' + modelFile
saxonArg = saxonPath + '/saxon.jar' 
reGenArg = '-xsl:' + reGenPath

executionParser = rePath + '/bin/re_execution_parser'
jsonOut = subprocess.check_output([executionParser, modelFile]).decode("utf-8")

jDeployment = json.loads(jsonOut)
for key in jDeployment["nodes"]:
    if key != "localhost":
        print("Non localhost node found, exiting!")
        sys.exit(1)


if not os.path.exists(codeDir+"/build"):
    os.makedirs(codeDir+"/build")

p = subprocess.run(["cmake", ".."], cwd=codeDir+"/build")
p = subprocess.run(["make", "-j6"], cwd=codeDir+"/build")

reArgs = [rePath + "/bin/re_node_manager"]
for key, val in jDeployment["nodes"]["localhost"]["re_node_manager"].items():
    reArgs.append("--" + key)
    reArgs.append(val)
    if key == "master":
        reArgs.append("-t")
        reArgs.append(str(executionTime))
        reArgs.append("-d" )
        reArgs.append(modelFile)

    if key == "slave":
        reArgs.append("-l")
        reArgs.append(".")

loggerArgs = [loganPath+"/bin/logan_server"]
for key, val in jDeployment["nodes"]["localhost"]["logan_server"].items():
    loggerArgs.append("--" + key)
    if isinstance(val, list):
        loggerArgs.extend(val)
    else:
        loggerArgs.append(val)

clientArgs = [loganPath+"/bin/logan_client"]
for key, val in jDeployment["nodes"]["localhost"]["logan_client"].items():
    clientArgs.append("--" + key)
    if isinstance(val, list):
        clientArgs.extend(val)
    else:
        clientArgs.append(str(val))

print(loggerArgs)
print(" ".join(loggerArgs))
print(clientArgs)
print(" ".join(clientArgs))
print(reArgs)
print(" ".join(reArgs))

serverProcess = subprocess.Popen(loggerArgs, preexec_fn=os.setpgrp)
clientProcess = subprocess.Popen(clientArgs, preexec_fn=os.setpgrp)
reProcess = subprocess.Popen(reArgs, cwd=codeDir+"/lib", preexec_fn=os.setpgrp)

while reProcess.poll() is None:
    time.sleep(1)

if not interrupted:
    reProcess.wait()
    print("waiting for re")
    clientProcess.terminate()
    clientProcess.wait()
    print("waiting for client")
    serverProcess.terminate()
    print("waiting for server")
    serverProcess.wait()

