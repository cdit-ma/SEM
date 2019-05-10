import glob, time, subprocess, os, sys

cd_stack = []
def push_cd(new_dir):
    global cd_stack
    cd_stack = [os.getcwd()] + cd_stack
    os.chdir(new_dir)

def pop_cd():
    global cd_stack
    new_dir = cd_stack.pop(0)
    os.chdir(new_dir)

def make_dir(dir):
    try:
        os.makedirs(dir)
    except:
        None

def run_command(args):
    start_time = time.time()
    success = True
    output = ""
    try:
        output = subprocess.check_output(args, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        success = False
        output = e.output
    return [success, output, time.time() - start_time]


def run_regen_generation(model_path):
    global RE_GEN_PATH
    args = ['java', '-jar', RE_GEN_PATH + 'saxon.jar', '-xsl:' + RE_GEN_PATH + 'generate_project.xsl', '-s:' + model_path]
    return run_command(args)

def run_cmake_generation():
    args = ['cmake', '-G', 'Ninja', '..']
    return run_command(args)

def run_cmake_build():
    args = ['cmake', '--build', '.']
    return run_command(args)


def get_model_name(model_path):
    base = os.path.basename(model_path)
    return os.path.splitext(base)[0]


def get_test_case(test_name, test_pass, test_time, fail_string = ""):
    args = 'name="' + test_name + '" status="run" time="' + str(test_time) + '"'
    tab = '\t\t'
    
    output = ""
    if(test_pass):
        output += tab + '<testcase ' + args + '/>\n'
    else:
        output += tab + '<testcase ' + args + '>\n'
        output += tab + '\t<failure message="' + fail_string + '" />\n'
        output += tab + '</testcase>\n'
    return output

def get_test_suite_open(suite_name, test_count, failure_count, test_time):
    return '\t<testsuite name="' + suite_name + '" tests="' + str(test_count) + '" failures="' + str(failure_count) + '" errors="0" disabled="0" time="' + str(test_time) + '">\n'

def get_test_suite_close():
    return "\t</testsuite>\n"

def get_test_suites_open(suite_name, test_count, failure_count, test_time):
    return '<testsuites name="' + suite_name + '" tests="' + str(test_count) + '" failures="' + str(failure_count) + '" errors="0" disabled="0" time="' + str(test_time) + '">\n'

def get_test_suites_close():
    return "</testsuites>\n"

def get_xml_preamble():
    return '<?xml version="1.0" encoding="UTF-8"?>\n'


try:
    RE_GEN_PATH = os.environ.get('RE_PATH') + '/re_gen/'
except:
    print("RE_PATH environment variable not set")
    sys.exit(-1)

all_suite_output = ""
total_tests = 0
total_fails = 0
total_time = 0

for model_path in glob.glob('models/**.graphml'):
    model_name = get_model_name(model_path)

    model_dir = "build/" + model_name
    make_dir(model_dir)
    push_cd(model_dir)

    gen_result = run_regen_generation("../../" + model_path)

    make_dir('build')
    push_cd('build')

    cmake_result = run_cmake_generation()
    build_result = run_cmake_build()

    # Get back out of the directory
    pop_cd()
    pop_cd()

    fail_count = int(not gen_result[0]) + int(not cmake_result[0]) + int(not build_result[0])
    test_time = gen_result[2] + cmake_result[2] + build_result[2]

    test_count = 3

    # Generate the suite output
    suite_output = get_test_suite_open('regen_' + model_name, 3, fail_count, test_time)
    suite_output += get_test_case("xsl_generation", gen_result[0], gen_result[2], gen_result[1])
    suite_output += get_test_case("cmake_generation", cmake_result[0], cmake_result[2], cmake_result[1])
    suite_output += get_test_case("compilation", build_result[0], build_result[2], build_result[1])
    suite_output += get_test_suite_close()

    # Update our top level counters
    all_suite_output += suite_output
    total_tests += test_count
    total_fails += fail_count
    total_time += test_time

    passed_count = test_count - fail_count
    print("Test: " + model_name + " [" + str(passed_count) + "/" + str(test_count) + "]" + (" FAILED!" if fail_count > 0 else ""))

# Write the Junit into a string
junit_output = get_xml_preamble()
junit_output += get_test_suites_open("regen_tests", total_tests, total_fails, total_time)
junit_output += all_suite_output
junit_output += get_test_suites_close()


output_file = 'output.xml'
if(len(sys.argv) == 2):
    # Get the output name
    output_file = sys.argv[1]

print("Writing JUnit: " + output_file)
f = open(output_file, "w")
f.write(junit_output)
f.close()
