#!/usr/bin/env python3

# This scripts runs GCC as well as IFCC on each test-case provided and compares the results.
#
# input: the test-cases are specified either as individual
#         command-line arguments, or as part of a directory tree
#
# output: 
#
# The script is divided in three distinct steps:
# - in the ARGPARSE step, we understand the command-line arguments
# - in the PREPARE step, we copy all our test-cases into a single directory tree
# - in the TEST step, we actually run GCC and IFCC on each test-case
#
#

import argparse
import os
import shutil
import sys
import subprocess
from tabulate import tabulate
import itertools

def command(string, logfile=None):
    """execute `string` as a shell command, optionnaly logging stdout+stderr to a file. return exit status.)"""
    if args.verbose:
        print("ifcc-test.py: "+string)
    try:
        output=subprocess.check_output(string,stderr=subprocess.STDOUT,shell=True)
        ret= 0
    except subprocess.CalledProcessError as e:
        ret=e.returncode
        output = e.output

    if logfile:
        f=open(logfile,'w')
        print(output.decode(sys.stdout.encoding)+'\n'+'exit status: '+str(ret),file=f)
        f.close()

    return ret

def dumpfile(name):
    print(open(name).read(),end='')

######################################################################################
## ARGPARSE step: make sense of our command-line arguments

argparser   = argparse.ArgumentParser(
description = "Compile multiple programs with both GCC and IFCC, run them, and compare the results.",
epilog      = ""
)

argparser.add_argument('input',metavar='PATH',nargs='+',help='For each path given:'
                       +' if it\'s a file, use this file;'
                       +' if it\'s a directory, use all *.c files in this subtree')

argparser.add_argument('-d','--debug',action="count",default=0,
                       help='Increase quantity of debugging messages (only useful to debug the test script itself)')
argparser.add_argument('-v','--verbose',action="count",default=0,
                       help='Increase verbosity level. You can use this option multiple times.')
argparser.add_argument('-w','--wrapper',metavar='PATH',
                       help='Invoke your compiler through the shell script at PATH. (default: `ifcc-wrapper.sh`)')
argparser.add_argument('-f','--failonly',action="count",default=0,
                       help='Only print the test-cases that failed. (default: print all test-cases)')

args=argparser.parse_args()

if args.debug >=2:
    print('debug: command-line arguments '+str(args))

orig_cwd=os.getcwd()
if "ifcc-test-output" in orig_cwd:
    print('error: cannot run from within the output directory')
    exit(1)
    
if os.path.isdir('ifcc-test-output'):
    # cleanup previous output directory
    command('rm -rf ifcc-test-output')
os.mkdir('ifcc-test-output')
    
## Then we process the inputs arguments i.e. filenames or subtrees
inputfilenames=[]
for path in args.input:
    path=os.path.normpath(path) # collapse redundant slashes etc.
    if os.path.isfile(path):
        if path[-2:] == '.c':
            inputfilenames.append(path)
        else:
            print("error: incorrect filename suffix (should be '.c'): "+path)
            exit(1)
    elif os.path.isdir(path):
        for dirpath,dirnames,filenames in os.walk(path):
            inputfilenames+=[dirpath+'/'+name for name in filenames if name[-2:]=='.c']
    else:
        print("error: cannot read input path `"+path+"'")
        sys.exit(1)

## debug: after treewalk
if args.debug:
    print("debug: list of files after tree walk:"," ".join(inputfilenames))

## sanity check
if len(inputfilenames) == 0:
    print("error: found no test-case in: "+" ".join(args.input))
    sys.exit(1)

## Here we check that  we can actually read the files.  Our goal is to
## fail as early as possible when the CLI arguments are wrong.
for inputfilename in inputfilenames:
    try:
        f=open(inputfilename,"r")
        f.close()
    except Exception as e:
        print("error: "+e.args[1]+": "+inputfilename)
        sys.exit(1)

## Last but not least: we now locate the "wrapper script" that we will
## use to invoke ifcc
if args.wrapper:
    wrapper=os.path.realpath(os.getcwd()+"/"+ args.wrapper)
else:
    wrapper=os.path.dirname(os.path.realpath(__file__))+"/ifcc-wrapper.sh"

if not os.path.isfile(wrapper):
    print("error: cannot find "+os.path.basename(wrapper)+" in directory: "+os.path.dirname(wrapper))
    exit(1)

if args.debug:
    print("debug: wrapper path: "+wrapper)
        
######################################################################################
## PREPARE step: copy all test-cases under ifcc-test-output
print("Preparing test cases...")

# Idle loading wheel
loading_wheel = itertools.cycle(["[\\]", "[|]", "[/]", "[-]"])

jobs=[]

for inputfilename in inputfilenames:
    sys.stdout.write("\r" + next(loading_wheel))
    sys.stdout.flush()
    if args.debug>=2:
        print("debug: PREPARING "+inputfilename)

    if 'ifcc-test-output' in os.path.realpath(inputfilename):
        print('error: input filename is within output directory: '+inputfilename)
        exit(1)
    
    ## each test-case gets copied and processed in its own subdirectory:
    subdir='../tests/ifcc-test-output/'+inputfilename.strip("./")[:-2]
    os.makedirs(subdir, exist_ok=True)
    shutil.copyfile(inputfilename, subdir+'/input.c')
    jobs.append(subdir)

## eliminate duplicate paths from the 'jobs' list
unique_jobs=[]
for j in jobs:
    sys.stdout.write("\r" + next(loading_wheel))
    sys.stdout.flush()
    for d in unique_jobs:
        if os.path.samefile(j,d):
            break # and skip the 'else' branch
    else:
        unique_jobs.append(j)
jobs=sorted(unique_jobs)

# debug: after deduplication
if args.debug:
    print("debug: list of test-cases after deduplication:"," ".join(jobs))


######################################################################################
## TEST step: actually compile all test-cases with both compilers
print("\n\nRunning test cases...")

nb_success=0
nb_failure=0
nb_jobs=len(jobs)
# Now we make a table with columns test path and test result, with color
data = []
x = 0
color_dict = {"red": "\033[91m",
		"green": "\033[92m",
		"yellow": "\033[93m",
		"na": ""
		}
# ANSI escape codes for the progress bar
reset_color = "\033[0m"
reset_cursor = "\033[1000D"
bar_length = 50


for jobname in jobs:
    # Progress bar
    width = int((x+1)/(len(jobs)/bar_length))
    bar = "[" + color_dict["green"] + "#" * width + " " * (bar_length - width) + reset_color + "]" + " " + str(x+1) + "/" + str(len(jobs))
    sys.stdout.write(reset_cursor + bar)
    sys.stdout.flush()
    x += 1

    os.chdir(orig_cwd)

    print('TEST-CASE: '+jobname.split('/')[-2]+'/'+jobname.split('/')[-1]) if args.verbose else None
    os.chdir(jobname)
    
    ## Reference compiler = GCC
    gccstatus=command("gcc -S -o asm-gcc.s input.c", "gcc-compile.txt")
    if gccstatus == 0:
        # test-case is a valid program. we should run it
        gccstatus=command("gcc -o exe-gcc asm-gcc.s", "gcc-link.txt")
    if gccstatus == 0: # then both compile and link stage went well
        exegccstatus=command("./exe-gcc", "gcc-execute.txt")
        if args.verbose >=2:
            dumpfile("gcc-execute.txt")
            
    ## IFCC compiler
    ifccstatus=command('"' + wrapper + '"' +" asm-ifcc.s input.c", "ifcc-compile.txt")
    
    if gccstatus != 0 and ifccstatus != 0:
        ## ifcc correctly rejects invalid program -> test-case ok
        print("TEST OK\r\n") if args.verbose else None 
        data.append([jobname.split('/')[-2]+'/'+jobname.split('/')[-1], "\033[32mOK\033[0m",gccstatus,ifccstatus, ""]) if not args.failonly else None
        nb_success+=1
        continue
    elif gccstatus != 0 and ifccstatus == 0:
        ## ifcc wrongly accepts invalid program -> error
        print("TEST FAIL (your compiler accepts an invalid program)\r\n") if args.verbose else None
        data.append([jobname.split('/')[-2]+'/'+jobname.split('/')[-1], "\033[31mFAIL\033[0m", gccstatus, ifccstatus,"your compiler accepts an invalid program"])
        nb_failure+=1
        continue
    elif gccstatus == 0 and ifccstatus != 0:
        ## ifcc wrongly rejects valid program -> error
        print("TEST FAIL (your compiler rejects a valid program)\r\n") if args.verbose else None
        data.append([jobname.split('/')[-2]+'/'+jobname.split('/')[-1], "\033[31mFAIL\033[0m", gccstatus, ifccstatus, "your compiler rejects a valid program"])
        nb_failure+=1
        if args.verbose:
            dumpfile("ifcc-compile.txt")
        continue
    else:
        ## ifcc accepts to compile valid program -> let's link it
        ldstatus=command("gcc -o exe-ifcc asm-ifcc.s", "ifcc-link.txt")
        if ldstatus:
            print("TEST FAIL (your compiler produces incorrect assembly)\r\n") if args.verbose else None
            data.append([jobname.split('/')[-2]+'/'+jobname.split('/')[-1], "\033[31mFAIL\033[0m", 0, ldstatus,"your compiler produces incorrect assembly"])
            nb_failure+=1
            if args.verbose:
                dumpfile("ifcc-link.txt")
            continue

    ## both compilers  did produce an  executable, so now we  run both
    ## these executables and compare the results.
        
    exeifccstatus = command("./exe-ifcc","ifcc-execute.txt")
    if open("gcc-execute.txt").read() != open("ifcc-execute.txt").read() :
        print("TEST FAIL (different results at execution)\r\n") if args.verbose else None
        data.append([jobname.split('/')[-2]+'/'+jobname.split('/')[-1], "\033[31mFAIL\033[0m", exegccstatus, exeifccstatus,"different results at execution"])
        nb_failure+=1
        if args.verbose:
            print("GCC:")
            dumpfile("gcc-execute.txt")
            print("you:")
            dumpfile("ifcc-execute.txt")
        continue

    ## last but not least
    print("TEST OK\r\n") if args.verbose else None
    data.append([jobname.split('/')[-2]+'/'+jobname.split('/')[-1], "\033[32mOK\033[0m", exegccstatus, exeifccstatus,""]) if not args.failonly else None
    nb_success+=1




table = tabulate(
    data, 
    headers=["Test Path", "Result", "gcc", "ifcc", "Comment"], 
    tablefmt="grid"
)
print()
print(table)
print("SUMMARY: "+str(nb_success)+" tests passed, "+str(nb_failure)+" tests failed, "+ str(nb_jobs)+" tests total")

