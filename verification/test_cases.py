import argparse
import json
import subprocess

def parse_arguments():
    parser = argparse.ArgumentParser(description='Run multiple test cases from a JSON configuration')
    parser.add_argument('config', type=str, help='Path to the JSON configuration file')
    
    input_dir_default = "testcase"
    parser.add_argument('-i', '--input-dir', type=str, default=input_dir_default, help=f'Input directory for stored testcases (default={input_dir_default})')
    
    log_dir_default = "log"
    parser.add_argument('-l', '--log-dir', type=str, default=log_dir_default, help=f'Log directory (default={log_dir_default})')
    
    temp_dir_default = "tmp"
    parser.add_argument('-t', '--temp-dir', type=str, default=temp_dir_default, help=f'Temporary directory (default={temp_dir_default})')
    
    target_default = "../while_ldb_parser/main"
    parser.add_argument('-T', '--target', type=str, default=target_default, help=f'Target executable for testing (default={target_default})')
    
    compile_default = "/usr/bin/g++"
    parser.add_argument('-c', '--compiler', type=str, default=compile_default, help=f'Path to g++ for cpp method (default={compile_default})')
    
    return parser.parse_args()

def test_onecase(test_case, args):
    cmd = [
        './test_onecase.py',
        '-n', test_case['name'],
        '-m', test_case['method'],
        '-i', args.input_dir,
        '-l', args.log_dir,
        '-t', args.temp_dir,
        '-T', args.target,
        '-c', args.compiler
    ]
    result = subprocess.run(cmd)
    return result.returncode

if __name__ == "__main__":
    args = parse_arguments()
    
    with open(args.config, 'r') as config_file:
        test_cases = json.load(config_file)
    
    summary = {
      "correct": 0,
      "incorrect": 0,
      "tester error": 0,
    }

    incorrect_list = []
    
    for test_case in test_cases:
        result = test_onecase(test_case, args)
        if result != 0:
            if result == 1:
                print(f"Test case '{test_case['name']}' failed due to incorrectness.")
                summary['incorrect'] += 1
                incorrect_list.append(test_case['name'])
            else:
                print(f"Test case '{test_case['name']}' failed due to a tester error.")
                summary['tester error'] += 1
        else:
            print(f"Test case '{test_case['name']}' passed successfully.")
            summary['correct'] += 1
            
    print(f"summary: {summary}")
    if len(incorrect_list):
      print(f"incorrect cases: ")
      for name in incorrect_list:
        print(name)
        
    exit(len(incorrect_list) != 0)
            