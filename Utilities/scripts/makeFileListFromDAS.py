#!/usr/bin/env python

import argparse
import subprocess
from python import UserInput
from python import ConfigureJobs
import logging
import os
import random

def getComLineArgs():
    parser = UserInput.getDefaultParser(False)
    parser.add_argument("-o", "--output_file", type=str,
                        required=True, help="Name of output text file")
    return vars(parser.parse_args())

def getDASFilesWithName(name, das_path):
    files = subprocess.check_output(["dasgoclient", "--query=file dataset=%s" % das_path])
    if files:
        files = filter(lambda x: "/store" in x[:7], files.split("\n"))
        files = ["@".join([name, f.strip()+'\n']) for f in files]
    return files

def makeFileListFromDAS(filenames, output_file, analysis, selection):
    name_path_map = ConfigureJobs.getListOfFilesWithDASPath(filenames, analysis, selection)

    files = []
    for name, path in name_path_map.iteritems():
        try:
            files.extend(getDASFilesWithName(name, path))
        except subprocess.CalledProcessError:
            logging.warning("Failed to find files for dataset %s with DAS path %s. Skipping!" % (name, path))
    
    # Randomize so one job doesn't tend to take longer than others
    random.shuffle(files)
    with open(output_file, "w") as outfile:
        outfile.writelines(files)

    return len(files)

def main():
    args = getComLineArgs()
    makeFileListFromDAS(args['filenames'], args['output_file'], args['analysis'], args['selection'])

if __name__ == "__main__":
    main()

