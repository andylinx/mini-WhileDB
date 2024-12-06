#!/usr/bin/env python
import argparse
import logging
import os
import subprocess
import filecmp
import shutil
import glob

"""
input_path:
<input-dir>/<name>/


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
    # expect to raise error or time-out
- induce
    + method.conf (containing one of cpp/output/error)
    + other materials required by induced method
    # read method.conf and apply specified method.
    
For all test modules:
    - Return 0 for matched results (correct).
    - Return 1 for mismatched results or runtime errors (incorrect).
    - Return -1 for non-testing errors (tester error).
"""

def parse_arguments():
    method_choices = ['cpp', 'output', 'error', 'induce']
    parser = argparse.ArgumentParser(description='Test one case script')
    parser.add_argument('-n', '--name', type=str, required=True, help='Test name')
    
    method_default = "induce"
    parser.add_argument('-m', '--method', type=str, default = method_default, choices=method_choices, help=f'Method (choice from {method_choices}, default={method_default})')
    
    input_dir_default = "testcase"
    parser.add_argument('-i', '--input-dir', type=str, default=input_dir_default, help=f'Input directory for stored testcases(default={input_dir_default})')
    
    log_dir_default = "log"
    parser.add_argument('--log-dir', type=str, default=log_dir_default, help=f'Log directory(default={log_dir_default})')
    
    tmp_dir_default = "tmp"
    parser.add_argument('--tmp-dir', type=str, default=tmp_dir_default, help=f'Temporary directory(default={tmp_dir_default})')
    
    target_default = "../while_ldb_parser/main"
    parser.add_argument('-T', '--target', type=str, default=target_default, help=f'Target executable for testing(default={target_default})')
    
    compile_default = "/usr/bin/g++"
    parser.add_argument('-c', '--compiler', type=str, default=compile_default, help=f'Path to g++ for cpp method(default={compile_default})')
    
    timeout_default = 1
    parser.add_argument('-t', '--timeout', type=int, default=timeout_default, help=f'Timeout threshold for subprocesses in seconds (default={timeout_default})')
    
    return parser.parse_args()

def setup_logger(name, log_dir):
    log_file = os.path.join(log_dir, f"{name}.log")
    os.makedirs(os.path.dirname(log_file), exist_ok=True)
    
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)
    
    file_handler = logging.FileHandler(log_file)
    file_handler.setLevel(logging.DEBUG)
    
    formatter = logging.Formatter('%(asctime)s - %(name)s - [%(levelname)s]: %(message)s')
    file_handler.setFormatter(formatter)
    
    logger.addHandler(file_handler)
    
    return logger

def compare_files(file1, file2, logger):
    try:
        result = subprocess.run(['diff', '-Zs', file1, file2], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode == 0:
            return True
        else:
            logger.error(f"Differences found:\n{result.stdout}")
            return False
    except subprocess.CalledProcessError as e:
        logger.error(f"Error running diff: {e}")
        return False

def test_cpp(args, logger):
    input_dir = args.input_dir
    tmp_dir = args.tmp_dir
    target = args.target
    compiler = args.compiler
    name = args.name
    timeout = args.timeout

    test_jtl = os.path.join(input_dir, name, 'test.jtl')
    test_cpp = os.path.join(input_dir, name, 'test.cpp')
    input_files = glob.glob(os.path.join(input_dir, name, '*.in'))
    output_out = os.path.join(tmp_dir, f'{name}.out')
    output_ref = os.path.join(tmp_dir, f'{name}.ref')
    compiled_cpp = os.path.join(tmp_dir, f'{name}_cpp_ref')
    null_input = os.path.join(tmp_dir, 'null.in')

    if not os.path.exists(test_jtl):
        logger.error(f"{test_jtl} does not exist.")
        return -1

    if not os.path.exists(test_cpp):
        logger.error(f"{test_cpp} does not exist.")
        return -1

    # Ensure the temporary directory exists
    os.makedirs(os.path.dirname(output_out), exist_ok=True)

    # Create a null input file if no input files exist
    if not input_files:
        with open(null_input, 'w') as f:
            pass
        input_files = [null_input]

    # Compile test.cpp
    try:
        result = subprocess.run([compiler, test_cpp, '-o', compiled_cpp], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        logger.info(f"Compiled {test_cpp} to {compiled_cpp}")
        logger.debug(result.stdout)
        logger.warning(result.stderr)
    except subprocess.CalledProcessError as e:
        logger.error(f"Error compiling {test_cpp}: {e.stderr}")
        return -1

    # Run target with test.jtl for each .in file
    for test_in in input_files:
        try:
            with open(test_in, 'r') as input_file, open(output_out, 'w') as output_file:
                subprocess.run([target, test_jtl], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE, check=True, timeout=timeout)
            logger.info(f"Executed {target} with {test_jtl} and {test_in}, output saved to {output_out}")
        except subprocess.TimeoutExpired:
            logger.error(f"Execution of {target} with {test_jtl} and {test_in} timed out.")
            return 1
        except subprocess.CalledProcessError as e:
            logger.error(f"Error running {target} with {test_jtl} and {test_in}: {e}")
            return 1

        # Run compiled_cpp with test.in
        try:
            with open(test_in, 'r') as input_file, open(output_ref, 'w') as output_file:
                subprocess.run([compiled_cpp], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE, check=True, timeout=timeout)
            logger.info(f"Executed {compiled_cpp} with {test_in}, output saved to {output_ref}")
        except subprocess.TimeoutExpired:
            logger.error(f"Execution of {compiled_cpp} with {test_in} timed out.")
            return -1
        except subprocess.CalledProcessError as e:
            logger.error(f"Error running {compiled_cpp} with {test_in}: {e}")
            return -1

        # Compare outputs
        if not compare_files(output_out, output_ref, logger):
            logger.error(f"Output does not match reference for {test_in}.")
            return 1

    logger.info("All test cases passed.")
    return 0

def test_output(args, logger):
    input_dir = args.input_dir
    tmp_dir = args.tmp_dir
    target = args.target
    name = args.name
    timeout = args.timeout

    test_jtl = os.path.join(input_dir, name, 'test.jtl')
    test_ref = os.path.join(input_dir, name, 'test.ref')
    test_in = os.path.join(input_dir, name, 'test.in')
    output_out = os.path.join(tmp_dir, f'{name}.out')
    output_ref = os.path.join(tmp_dir, f'{name}.ref')

    if not os.path.exists(test_jtl):
        logger.error(f"{test_jtl} does not exist.")
        return -1

    if not os.path.exists(test_ref):
        logger.error(f"{test_ref} does not exist.")
        return -1

    # Ensure the temporary directory exists
    os.makedirs(tmp_dir, exist_ok=True)

    # Copy test.ref to tmp/<name>.ref
    shutil.copy(test_ref, output_ref)
    logger.info(f"Copied {test_ref} to {output_ref}")

    # Run target with test.jtl
    try:        
        input_file = open(test_in, 'r') if os.path.exists(test_in) else None
        output_file = open(output_out, 'w')
        subprocess.run([target, test_jtl], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE, check=True, timeout=timeout)
        if input_file:
            input_file.close()
        output_file.close()
        logger.info(f"Executed {target} {test_jtl}, output saved to {output_out}")
    except subprocess.TimeoutExpired:
        logger.error(f"Execution of {target} {test_jtl} timed out.")
        return 1
    except subprocess.CalledProcessError as e:
        logger.error(f"Error running {target} {test_jtl}: {e}")
        if input_file:
            input_file.close()
        output_file.close()
        return 1

    # Compare outputs
    if compare_files(output_out, output_ref, logger):
        logger.info("Output matches reference.")
        return 0
    else:
        logger.error("Output does not match reference.")
        return 1

def test_error(args, logger):
    input_dir = args.input_dir
    tmp_dir = args.tmp_dir
    target = args.target
    name = args.name
    timeout = args.timeout

    test_jtl = os.path.join(input_dir, name, 'test.jtl')
    test_in = os.path.join(input_dir, name, 'test.in')
    output_out = os.path.join(tmp_dir, f'{name}.out')

    if not os.path.exists(test_jtl):
        logger.error(f"{test_jtl} does not exist.")
        return -1

    # Ensure the temporary directory exists
    os.makedirs(tmp_dir, exist_ok=True)

    # Run target with test.jtl
    try:
        input_file = open(test_in, 'r') if os.path.exists(test_in) else None
        output_file = open(output_out, 'w')
        result = subprocess.run([target, test_jtl], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE, timeout=timeout)
        if input_file:
            input_file.close()
        output_file.close()
        logger.info(f"Executed {target} {test_jtl}, output saved to {output_out}")
        
        if result.returncode == 1:
            logger.info(f"Expected error occurred with return code 1 while running {target} {test_jtl}")
            return 0
        elif result.returncode == 0:
            logger.error(f"Expected error did not occur and {target} {test_jtl} exited with 0 normally")
            return 1
        else:
            logger.error(f"Expected return code 1 but got {result.returncode} while running {target} {test_jtl}")
            return 1
    except subprocess.TimeoutExpired: # time out is also considered error expected
        logger.info(f"Execution of {target} {test_jtl} timed out (as expected).")
        return 0
    except subprocess.CalledProcessError as e:
        if input_file:
            input_file.close()
        output_file.close()
        logger.error(f"Unexpected error occurred while running {target} {test_jtl}: {e}")
        return -1
    
if __name__ == "__main__":
    args = parse_arguments()
    
    logger = setup_logger(args.name, args.log_dir)
    
    logger.info("----------------BEGINNING OF TEST----------------")
    
    if args.method == "induce":
        # open file method.conf and read the method
        method_conf_path = os.path.join(args.input_dir, args.name, 'method.conf')
        if not os.path.exists(method_conf_path):
            logger.error(f"{method_conf_path} does not exist.")
            exit(-1)
        
        with open(method_conf_path, 'r') as f:
            induced_method = f.read().strip()
        
        if induced_method not in ['cpp', 'output', 'error']:
            logger.error(f"Invalid method specified in {method_conf_path}: {induced_method}")
            exit(-1)
        
        args.method = induced_method

    outcome = -1
    if args.method == "cpp":
        outcome = test_cpp(args, logger)
    elif args.method == "output":
        outcome = test_output(args, logger)
    else:
        outcome = test_error(args, logger)
    
    logger.info("----------------END OF TEST----------------")
    
    exit(outcome)