#!/usr/bin/python

# Run test cases specified in json.
# for json format, see sample.json. (if method not specified, default to "induce")
"""
[
  {
      "name": "tester_self_test/cpp",
      "method": "cpp"
  },
  {
      "name": "tester_self_test/output",
      "method": "output"
  },
  {
      "name": "tester_self_test/error",
      "method": "error"
  },
  {
      "name": "tester_self_test/induce"
  }
]
"""
# for test case layout, see test_onecase.py
"""
input_path:
<input-dir>/<name>/*
output_path:
<output_dir>/<name>.*

method:
- cpp
    + test.jtl
    + test.cpp
    + *.in (optional, if not provided, assume no input)
    # compare with equivalent cpp output
- output
    + test.jtl
    + test.in (optional, if not provided, assume no input)
    + test.ref
    # compare output with test.ref
- error
    + test.jtl
    + test.in (optional)
    # expect to raise error or time-out (but not syntax error)
- induce
    + method.conf (containing one of cpp/output/error)
    + other materials required by induced method
    # read method.conf and apply specified method.
    
For all test modules:
    - Return 0 for matched results (correct).
    - Return 1 for mismatched results or runtime errors (incorrect).
    - Return -1 for non-testing errors (tester error).
"""

import argparse
import json
import subprocess
import os

def parse_arguments():
    parser = argparse.ArgumentParser(description='Run multiple test cases from a JSON configuration')
    parser.add_argument('config', type=str, help='Path to the JSON configuration file')
    
    input_dir_default = "cases"
    parser.add_argument('-i', '--input-dir', type=str, default=input_dir_default, help=f'Input directory for stored testcases (default={input_dir_default})')
    
    log_dir_default = "log"
    parser.add_argument('--log-dir', type=str, default=log_dir_default, help=f'Log directory (default={log_dir_default})')
    
    tmp_dir_default = "tmp"
    parser.add_argument('--tmp-dir', type=str, default=tmp_dir_default, help=f'Temporary directory (default={tmp_dir_default})')
    
    target_default = "../while_ldb_parser/main"
    parser.add_argument('-T', '--target', type=str, default=target_default, help=f'Target executable for testing (default={target_default})')
    
    compile_default = "/usr/bin/g++"
    parser.add_argument('-c', '--compiler', type=str, default=compile_default, help=f'Path to g++ for cpp method (default={compile_default})')
    
    timeout_default = 1
    parser.add_argument('-t', '--timeout', type=int, default=timeout_default, help=f'Timeout threshold for subprocesses in seconds (default={timeout_default})')
    
    return parser.parse_args()

def test_onecase(test_case, args):
    # Get the directory of the current script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    test_onecase_path = os.path.join(script_dir, 'test_onecase.py')
    
    cmd = [
        'python3',  # Explicitly use python3
        test_onecase_path,  # Use absolute path
        '-n', test_case['name'],
        '-m', test_case.get("method", "induce"),
        '-i', args.input_dir,
        '--log-dir', args.log_dir,
        '--tmp-dir', args.tmp_dir,
        '-T', args.target,
        '-c', args.compiler,
        '-t', str(args.timeout)
    ]
    try:
        result = subprocess.run(cmd)
    except Exception as e:
        print(f"Tester exception detected on test case {test_case['name']}: {e}")
        return -1
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
    tester_error_list = []
    
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
                tester_error_list.append(test_case['name'])
        else:
            print(f"Test case '{test_case['name']}' passed successfully.")
            summary['correct'] += 1
            
    print(f"Summary: {summary}")
    if len(incorrect_list):
      print(f"Incorrect cases: ")
      for name in incorrect_list:
        print(f"> {name}")
    
    if len(tester_error_list):
      print(f"Tester error cases: ")
      for name in tester_error_list:
        print(f"> {name}")
    
        
    exit(len(incorrect_list) != 0 or len(tester_error_list) != 0)
