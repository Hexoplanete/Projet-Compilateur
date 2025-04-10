#!/usr/bin/env python3
"""
This scripts runs GCC as well as a custom compiler on each test-case provided and compares the results.

Each test generates its own folder containing the result of the compilation, linking and execution of the test.
At the end, the results of the run are stored in a csv

run `ifcc-test.py -h` to see the list of arguments
"""

import argparse
import csv
from datetime import datetime
import logging
from dataclasses import dataclass
import os
import shutil
import sys
import subprocess
import glob
import re

logger = logging.getLogger("ifcc")

def command(string, logfile=None):
    """execute `string` as a shell command, optionally logging stdout+stderr to a file. return exit status.)"""
    try:
        logger.debug(f"sh: {string} > '{logfile}'")
        output = subprocess.check_output(
            string, stderr=subprocess.STDOUT, shell=True)
        ret = 0
    except subprocess.CalledProcessError as e:
        ret = e.returncode
        output = e.output

    if logfile:
        f = open(logfile, "w")
        print(f"{output.decode(sys.stdout.encoding)}\nexit status: {ret}", file=f)
        f.close()

    return ret


def tabulate(data: list[list], headers: list[str]):
    lines = []
    col_width = [len(i) for i in headers]
    align = ["<" for _ in headers]
    for line in data:
        for i, entry in enumerate(list(line)):
            if type(entry) is not str:
                align[i] = ">"
                entry = str(entry)
            else:
                entry = re.sub(r"\033\[(\d|;)+?m", "", entry)
            col_width[i] = max(col_width[i], len(entry))

    sep = "+-" + "-+-".join(["-"*w for w in col_width]) + "-+"

    def format_line(line: list):
        entries = []
        for (entry, a, w) in zip(line, align, col_width):
            if type(entry) is str:
                raw_entry = re.sub(r"\033\[(\d|;)+?m", "", entry)
                w += len(entry) - len(raw_entry)
            entries.append(f"{{0: {a}{w}}}".format(entry))
        return "| " + " | ".join(entries) + " |"

    lines.append(sep)
    lines.append(format_line(headers))
    lines.append(sep.replace("-", "="))
    for line in data:
        lines.append(format_line(line))
        lines.append(sep)

    return "\n".join(lines)


def parse_args(args: list[str] | None = None):
    arg_parser = argparse.ArgumentParser(
        description="Compile multiple programs with both GCC and a custom compiler, run them, and compare the results.",
    )

    arg_parser.add_argument("-l", "--log", default="WARNING", choices=[
        "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"], help="Set the logging level (Defaults to 'WARNING')")
    arg_parser.add_argument("tests", metavar="PATH", nargs="+",
                            help="For each path given: if it\"s a file, use this file; if it\"s a directory, use all *.c files in this subtree")
    arg_parser.add_argument("-c", "--compiler-path", default="../ifcc",
                            help="The path to the compiler to test (Defaults to 'ifcc')")
    arg_parser.add_argument("-o", "--output", default=None,
                            help="The output directory for the tests. The csv will have the name '<datetime>-<output>.csv' (Defaults to 'ifcc-test-<compilername>')")
    arg_parser.add_argument("-f", "--failed_only", action="store_true",
                            default=False, help="Only print the test-cases that failed")
    
    arg_parser.add_argument("--no-table", action="store_true",
                            default=False, help="Do not print the summary table at the end")
    arg_parser.add_argument("--no-csv", action="store_true",
                            default=False, help="Do not save the results in a csv")

    return arg_parser.parse_args(args=args)


def setup_logging(level: str):
    level = level.upper()
    logging.basicConfig(level=level, format="[%(levelname)s] %(msg)s")
    logger.debug(f"Set log level to {level}")


@dataclass
class Compiler:
    name: str
    path: str
    compile_flags: str = ""


def check_compiler(path: str) -> Compiler:
    logger.info(f"Checking compiler '{path}'...")
    if not os.path.isabs(path):
        path = os.path.join(os.getcwd(), path)
    if not os.path.isfile(path):
        logger.error(f"Could not find '{path}'")
        exit(1)
    name = os.path.basename(path)
    logger.info(f"Using compiler {name} ('{path}')")
    return Compiler(name=name, path=path)

@dataclass
class TestCase:
    name: str
    path: str

def list_test_files(paths: list[TestCase]) -> list[TestCase]:
    print("Listing test files...")
    test_files: list[TestCase] = []
    for test, path in enumerate(paths):
        logger.info(f"({test+1}/{len(paths)}) '{path}'")
        path: str = os.path.normpath(path)  # collapse redundant slashes etc.
        if os.path.isfile(path):
            if os.path.splitext(path)[1] == ".c":
                test_files.append(TestCase(os.path.splitext(path)[0], path))
            else:
                logger.error(f"incorrect filename suffix (should be '.c'): '{path}'")
                exit(1)
        elif os.path.isdir(path):
            for dirpath, _, filenames in os.walk(path):
                for filename in [f for f in filenames if os.path.splitext(f)[1] == ".c"]:
                    file = os.path.join(dirpath, filename)
                    name = os.path.splitext(file)[0].removeprefix(f"{path}/")
                    test_files.append(TestCase(name,file))

        else:
            logger.error(f"cannot read input path '{path}'")
            sys.exit(1)

    logger.debug(f"Found {len(test_files)} tests files")

    if len(test_files) == 0:
        print(f"Nothing to test in {paths}")
        sys.exit(1)

    logger.debug(f"Attempting to open the files...")
    for i, test in enumerate(test_files):
        logger.debug(f"({i+1}/{len(test_files)}) '{test.path}'")
        try:
            f = open(test.path, "r")
            f.close()
        except Exception as e:
            logger.error(f"{e.args[1]}: {test.path}")
            sys.exit(1)

    logger.debug("Removing duplicates...")
    unique_test_files: list[TestCase] = []
    for i, test in enumerate(test_files):
        logger.debug(f"({i+1}/{len(test_files)}) {test.name}")
        for utf in unique_test_files:
            if os.path.samefile(test.path, utf.path):
                logger.warning(
                    f"Tests {test.name} and {utf.name} are the same. Only {utf.name} will be tested")
                break
        else:
            unique_test_files.append(test)

    print(f"Found {len(unique_test_files)} tests files")
    return sorted(unique_test_files, key=lambda t:t.name)


def prepare_tests(output: str, test_files: list[TestCase]):
    if os.path.isdir(output):
        logger.info(f"Cleaning previous run...")
        logger.debug(f"sh: rm -rf '{output}'")
        shutil.rmtree(output)
        logger.info(f"Done")
    
    print(f"Preparing tests...")
    logger.debug(f"sh: mkdir '{output}'")
    os.makedirs(output, exist_ok=True)

    tests:list[TestCase] = []
    for i, test in enumerate(test_files):
        logger.info(f"({i+1}/{len(test_files)}) {test.name}")
        folder = os.path.splitext(test.path)[0]

        # each test-case gets copied and processed in its own subdirectory:
        subdir = os.path.join(output, folder)
        logger.debug(f"sh: mkdir '{subdir}'")
        os.makedirs(subdir, exist_ok=True)
        logger.debug(f"sh: cp '{test.path}' '{os.path.join(subdir, 'input.c')}'")
        shutil.copyfile(test.path, os.path.join(subdir, "input.c"))
        tests.append(TestCase(test.name, subdir))

    print(f"Done")
    return tests


class RunResult:
    compile: int = -1
    link: int = -1
    execute: int = -1


def test_compiler(compiler: Compiler, file: str):
    results = RunResult()

    asm_name = f"0_asm-{compiler.name}"
    results.compile = command(f'"{compiler.path}" {compiler.compile_flags} "{file}" -o {asm_name}.s', f'0_compile-{compiler.name}.txt')
    if results.compile != 0:
        return results

    bin_name = f"1_bin-{compiler.name}"
    results.link = command(f'gcc {asm_name}.s -o {bin_name}', f'1_link-{compiler.name}.txt')
    if results.link != 0:
        return results

    results.execute = command(f'./{bin_name}', f'2_execute-{compiler.name}.txt')
    return results


@dataclass
class TestResult:
    name: str
    passed: bool
    step: str
    gcc: int
    compiler: int
    comment: str = ""


def passed_color(passed: bool):
    return "\033[32mOK\033[0m" if passed else "\033[31mFAIL\033[0m"


GCC = Compiler("gcc", "gcc", compile_flags="-S")


def run_tests(tests: list[TestCase], compiler: Compiler):
    print("Running tests...")
    results: list[TestResult] = []

    orig_cwd = os.getcwd()

    for i, test in enumerate(tests):
        logger.info(f"({i+1}/{len(tests)}) {test.name}")
        os.chdir(orig_cwd)
        os.chdir(test.path)

        def save_result(passed: bool, step: str, gcc_res: int, compiler_res: int, comment: str = ""):
            results.append(TestResult(test.name, passed, step, gcc_res, compiler_res, comment))
            if passed:
                print(f"({i+1}/{len(tests)}) {test.name}: {passed_color(passed)}")
            else:
                print(f"({i+1}/{len(tests)}) {test.name}: {passed_color(passed)} - {step} - {GCC.name}: {gcc_res}, {compiler.name}: {compiler_res} - {comment}")

        gcc_result = test_compiler(GCC, "input.c")
        compiler_results = test_compiler(compiler, "input.c")

        if gcc_result.compile != 0 and compiler_results.compile != 0:
            # ifcc correctly rejects invalid program -> test-case ok
            save_result(True, "Compile", gcc_result.compile, compiler_results.compile)
            continue
        elif gcc_result.compile != 0 and compiler_results.compile == 0:
            # ifcc wrongly accepts invalid program -> error
            save_result(False, "Compile", gcc_result.compile, compiler_results.compile, "your compiler accepts an invalid program")
            continue
        elif gcc_result.compile == 0 and compiler_results.compile != 0:
            # ifcc wrongly rejects valid program -> error
            save_result(False, "Compile", gcc_result.compile, compiler_results.compile, "your compiler rejects a valid program")
            continue

        # ifcc accepts to compile valid program -> let's link it
        if gcc_result.link != 0 and compiler_results.link != 0:
            save_result(True, "Link", gcc_result.link, compiler_results.link)
            continue

        if compiler_results.link != 0:
            save_result(False, "Link", gcc_result.link, compiler_results.link, "your compiler produces incorrect assembly")
            continue

        # both compilers  did produce an  executable, so now we  run both
        # these executables and compare the results.
        if gcc_result.execute != compiler_results.execute:
            save_result(False, "Exe", gcc_result.execute, compiler_results.execute, "different results at execution")
            continue

        save_result(True, "Exe", gcc_result.execute, compiler_results.execute)
    os.chdir(orig_cwd)
    return results

@dataclass
class ResultsStats:
    total: int
    passed: int

    @property
    def failed(self): return self.total - self.passed
    @property
    def coverage(self): return self.passed/self.total


def compute_stats(results: list[TestResult]):
    logger.debug("Computing stats...")
    passed = len(list(filter(lambda r: r.passed, results)))
    logger.debug("Done")
    return ResultsStats(len(results), passed)


def log_results(compiler:Compiler, results: list[TestResult], stats: ResultsStats, no_table:bool, failed_only: bool):
    failed = list(filter(lambda r: not r.passed, results))

    if not no_table:
        logger.debug("Generating table...")
        source = failed if failed_only else results
        headers = ["Test", "Result", "Step", "gcc", compiler.name, "Comment"]
        data: list = [[r.name, passed_color(r.passed), r.step, r.gcc, r.compiler, r.comment] for r in source]
        table = tabulate(data, headers)
        logger.debug("Done")
        print(table)

    print(f"{compiler.name} passed {stats.passed}/{stats.total} tests ({stats.coverage*100:.2f}%)")


def save_results(output: str, compiler: Compiler, results: list[TestResult], stats: ResultsStats):
    file_name = f"{datetime.today().strftime('%Y%m%d_%H%M%S')}-{output}.csv"
    logger.info(f"Saving results...")
    with open(file_name, 'w') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(["Test", "Result", "Step", "gcc", compiler.name, "Comment"])
        for r in results:
            row = [r.name, "OK" if r.passed else "FAIL", r.step, r.gcc, r.compiler, r.comment]
            writer.writerow(row)
    print(f"Saved results to '{file_name}'")


if __name__ == "__main__":
    args = parse_args()

    setup_logging(args.log)

    compiler = check_compiler(args.compiler_path)
    if args.output is None: args.output = f"ifcc-test-{compiler.name}" 

    test_files = list_test_files(args.tests)

    tests = prepare_tests(args.output, test_files)

    results = run_tests(tests, compiler)

    stats = compute_stats(results)

    log_results(compiler, results, stats, args.no_table, args.failed_only)

    if not args.no_csv:
        save_results(args.output, compiler, results, stats)

