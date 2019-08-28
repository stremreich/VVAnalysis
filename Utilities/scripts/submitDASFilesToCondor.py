#!/usr/bin/env python

import argparse
import makeFileListFromDAS as makeDAS
from python import UserInput
from python import ConfigureJobs
import logging
import os
import shutil
import glob
import tarfile
import math

def getComLineArgs():
    parser = UserInput.getDefaultParser(False)
    parser.add_argument("-d", "--submit_dir", type=str,
                        required=True, help="Output directory")
    parser.add_argument("-n", "--files_per_job", type=int,
                        default=3, help="Number of files per job")
    parser.add_argument("--nosubmit", action='store_true',
                        help="Just make the directory, don't submit")
    parser.add_argument("--input_tier", type=str,
        default="", help="Selection stage of input files")
    return vars(parser.parse_args())

def makeSubmitDir(submit_dir):
    log_dir = submit_dir + "/logs"
    if os.path.isdir(submit_dir):
        logging.warning("Overwriting directory %s" % submit_dir)
        shutil.rmtree(submit_dir)
    os.makedirs(log_dir)

def copyLibs():
    libdir = "lib"
    if os.path.isdir(libdir):
        shutil.rmtree(libdir)
    os.mkdir(libdir)
    
    cmssw_libdir = "/".join([os.environ["CMSSW_BASE"], libdir, os.environ["SCRAM_ARCH"], "*VVAnalysis*"])
    for i in glob.glob(cmssw_libdir):
        shutil.copyfile(i, '/'.join([libdir, os.path.basename(i)]))

def copyDatasetManagerFiles(analysis):
    manager_name = "AnalysisDatasetManager"
    manager_path = ConfigureJobs.getManagerPath()

    if os.path.isdir(manager_name):
        shutil.rmtree(manager_name)

    info_dir = manager_name+"/FileInfo"
    plot_dir = manager_name+"/PlotObjects"
    os.makedirs(info_dir)
    os.makedirs(plot_dir)

    for paths in [[manager_path, manager_name, "Utilities"],
                    [manager_path, info_dir, "data"],
                    [manager_path, info_dir, "montecarlo"],
                    [manager_path, plot_dir, analysis]]:
        path = '/'.join(paths)
        if os.path.isdir(path):
            shutil.copytree(path, '/'.join(paths[1:]))
        else:
            for d in glob.glob(path+'*'):
                shutil.copy(d, d.replace(manager_path+"/", ""))

def copyGridCertificate():
    # TODO: Check that it's valid for enough time
    shutil.copy("/tmp/x509up_u%s" % os.getuid(), "userproxy")

def tarAnalysisInfo(condor_dir, tarball_name):
    tarname = condor_dir+"/"+tarball_name
    with tarfile.open(tarname, "w:gz") as tar:
        tar.add("Utilities")
        tar.add("data")
        tar.add("lib")
        tar.add("AnalysisDatasetManager")
        tar.add("userproxy")
    shutil.rmtree("lib")
    shutil.rmtree("AnalysisDatasetManager")
    os.remove("userproxy")

def writeSubmitFile(submit_dir, analysis, selection, input_tier, numfiles, nPerJob):
    template_dict = {
        "analysis" : analysis,
        "selection" : selection,
        "input_tier" : input_tier,
        "nPerJob" : nPerJob,
        "nJobs" : int(math.ceil(numfiles/nPerJob)),
        "extraArgs" : "--test", # TODO: This should be removed soon!
    }

    template = "Templates/CondorSubmit/submit_template.jdl"
    outfile = "/".join([submit_dir, "submit.jdl"])
    ConfigureJobs.fillTemplatedFile(template, outfile, template_dict)

def writeWrapperFile(submit_dir, tarball_name):
    template_dict = { 
            "CMSSW_RELEASE_BASE" : os.environ["CMSSW_RELEASE_BASE"],
            "tarball" : tarball_name
    }
    template = "Templates/CondorSubmit/wrapRunSelector.sh"
    outfile = "/".join([submit_dir, "wrapRunSelector.sh"])
    ConfigureJobs.fillTemplatedFile(template, outfile, template_dict)

def submitDASFilesToCondor(filenames, submit_dir, analysis, selection, input_tier, numPerJob):
    makeSubmitDir(submit_dir)
    copyLibs()
    copyDatasetManagerFiles(analysis)
    copyGridCertificate()

    filelist = submit_dir + "/filelist.txt"
    numfiles = makeDAS.makeFileListFromDAS(filenames, filelist, analysis, input_tier)
    writeSubmitFile(submit_dir, analysis, selection, input_tier, numfiles, numPerJob)

    tarball_name = '_'.join([analysis, "AnalysisCode.tgz"])
    writeWrapperFile(submit_dir, tarball_name)
    tarAnalysisInfo(submit_dir, tarball_name)

def main():
    args = getComLineArgs()
    submitDASFilesToCondor(args['filenames'], args['submit_dir'], args['analysis'], 
        args['selection'], args['input_tier'], args['files_per_job'])

if __name__ == "__main__":
    main()
