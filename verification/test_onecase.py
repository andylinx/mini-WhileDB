#!/usr/bin/env python
import argparse
import logging
import os
import subprocess
import filecmp
import shutil

"""
input:
<input-dir>/<name>/*

method:
- cpp
    + test.jtl
    + test.cpp
    + test.in (optional)
    # compare with equivalent cpp outcome
- output
    + test.jtl
    + test.in (optional)
    + test.ref
    # compare output with test.ref
- error
    + test.jtl
    + test.in (optional)
    # expect to raise error
    
For all test modules:
    - Return 0 for matched results (correct).
    - Return 1 for mismatched results or runtime errors (incorrect).
    - Return -1 for non-testing errors (tester error).
"""

def parse_arguments():
    method_choices = ['cpp', 'output', 'error']
    parser = argparse.ArgumentParser(description='Test one case script')
    parser.add_argument('-n', '--name', type=str, required=True, help='Test name')
    parser.add_argument('-m', '--method', type=str, choices=method_choices, required=True, help=f'Method (choice from {method_choices})')
    
    input_dir_default = "testcase"
    parser.add_argument('-i', '--input-dir', type=str, default=input_dir_default, help=f'Input directory for stored testcases(default={input_dir_default})')
    
    log_dir_default = "log"
    parser.add_argument('-l', '--log-dir', type=str, default=log_dir_default, help=f'Log directory(default={log_dir_default})')
    
    temp_dir_default = "tmp"
    parser.add_argument('-t', '--temp-dir', type=str, default=temp_dir_default, help=f'Temporary directory(default={temp_dir_default})')
    
    target_default = "../while_ldb_parser/main"
    parser.add_argument('-T', '--target', type=str, default=target_default, help=f'Target executable for testing(default={target_default})')
    
    compile_default = "/usr/bin/g++"
    parser.add_argument('-c', '--compiler', type=str, default=compile_default, help=f'Path to g++ for cpp method(default={compile_default})')
    
    return parser.parse_args()

def setup_logger(name, log_dir):
    os.makedirs(log_dir, exist_ok=True)
    log_file = os.path.join(log_dir, f"{name}.log")
    
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
    tmp_dir = args.temp_dir
    target = args.target
    compiler = args.compiler
    name = args.name

    test_jtl = os.path.join(input_dir, name, 'test.jtl')
    test_cpp = os.path.join(input_dir, name, 'test.cpp')
    test_in = os.path.join(input_dir, name, 'test.in')
    output_out = os.path.join(tmp_dir, f'{name}.out')
    output_ref = os.path.join(tmp_dir, f'{name}.ref')
    compiled_cpp = os.path.join(tmp_dir, f'{name}_cpp_ref')

    if not os.path.exists(test_jtl):
        logger.error(f"{test_jtl} does not exist.")
        return -1

    if not os.path.exists(test_cpp):
        logger.error(f"{test_cpp} does not exist.")
        return -1

    # Ensure the temporary directory exists
    os.makedirs(tmp_dir, exist_ok=True)

    # Run target with test.jtl
    try:
        logger.info(f"Executing {target} with {test_jtl}")
        input_file = open(test_in, 'r') if os.path.exists(test_in) else None
        output_file = open(output_out, 'w')
        subprocess.run([target, test_jtl], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE, check=True)
        if input_file:
            input_file.close()
        output_file.close()
        logger.info(f"Executed {target} with {test_jtl}, output saved to {output_out}")
    except subprocess.CalledProcessError as e:
        logger.error(f"Error running {target} with {test_jtl}: {e}")
        if input_file:
            input_file.close()
        output_file.close()
        return 1

    # Compile test.cpp
    try:
        logger.info(f"Compiling {test_cpp}")
        subprocess.run([compiler, test_cpp, '-o', compiled_cpp], check=True)
        logger.info(f"Compiled {test_cpp} to {compiled_cpp}")
    except subprocess.CalledProcessError as e:
        logger.error(f"Error compiling {test_cpp}: {e}")
        return -1

    # Run compiled_cpp with test.in
    try:
        logger.info(f"Executing {compiled_cpp}")
        input_file = open(test_in, 'r') if os.path.exists(test_in) else None
        output_file = open(output_ref, 'w')
        subprocess.run([compiled_cpp], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE, check=True)
        if input_file:
            input_file.close()
        output_file.close()
        logger.info(f"Executed {compiled_cpp}, output saved to {output_ref}")
    except subprocess.CalledProcessError as e:
        logger.error(f"Error running {compiled_cpp}: {e}")
        if input_file:
            input_file.close()
        output_file.close()
        return -1

    # Compare outputs
    if compare_files(output_out, output_ref, logger):
        logger.info("Output matches reference.")
        return 0
    else:
        logger.error("Output does not match reference.")
        return 1

def test_output(args, logger):
    input_dir = args.input_dir
    tmp_dir = args.temp_dir
    target = args.target
    name = args.name

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
        logger.info(f"Executing {target} with {test_jtl}")
        input_file = open(test_in, 'r') if os.path.exists(test_in) else None
        output_file = open(output_out, 'w')
        subprocess.run([target, test_jtl], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE, check=True)
        if input_file:
            input_file.close()
        output_file.close()
        logger.info(f"Executed {target} with {test_jtl}, output saved to {output_out}")
    except subprocess.CalledProcessError as e:
        logger.error(f"Error running {target} with {test_jtl}: {e}")
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
    tmp_dir = args.temp_dir
    target = args.target
    name = args.name

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
        logger.info(f"Executing {target} with {test_jtl}")
        input_file = open(test_in, 'r') if os.path.exists(test_in) else None
        output_file = open(output_out, 'w')
        result = subprocess.run([target, test_jtl], stdin=input_file, stdout=output_file, stderr=subprocess.PIPE)
        if input_file:
            input_file.close()
        output_file.close()
        logger.info(f"Executed {target} with {test_jtl}, output saved to {output_out}")
        
        if result.returncode == 1:
            logger.info(f"Expected error occurred with return code 1 while running {target} with {test_jtl}")
            return 0
        elif result.returncode == 0:
            logger.error(f"Expected error did not occur and {target} with {test_jtl} exited with 0 normally")
            return 1
        else:
            logger.error(f"Expected return code 1 but got {result.returncode} while running {target} with {test_jtl}")
            return 1
    except subprocess.CalledProcessError as e:
        if input_file:
            input_file.close()
        output_file.close()
        logger.error(f"Unexpected error occurred while running {target} with {test_jtl}: {e}")
        return -1
if __name__ == "__main__":
    args = parse_arguments()
    
    logger = setup_logger(args.name, args.log_dir)
    
    logger.info("----------------BEGINNING OF TEST----------------")
    
    outcome = -1
    if args.method == "cpp":
        outcome = test_cpp(args, logger)
    elif args.method == "output":
        outcome = test_output(args, logger)
    else:
        outcome = test_error(args, logger)
    
    # # Temporarily bypass logger's format
    # with open(log_file_handler.baseFilename, 'a') as log_file:
    #     log_file.write("----------------END OF TEST----------------\n")
    
        
    logger.info("----------------END OF TEST----------------")
    
    exit(outcome)