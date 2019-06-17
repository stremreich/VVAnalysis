#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os
import multiprocessing
import subprocess

class SelectorDriver(object):
    def __init__(self, analysis, selection, input_tier):
        selector_map = {
            "WZxsec2016" : "WZSelector",
            "Zstudy" : "ZSelector",
            "Zstudy_2016" : "ZSelector",
            "Zstudy_2017" : "ZSelector",
            "ZZGen" : "ZZGenSelector",
        }

        self.analysis = analysis
        self.selection = selection
        self.input_tier = input_tier
        if analysis not in selector_map.keys():
            raise ValueError("Analysis does not point to " \
                "a defined selector. Please edit "
                "Utilities/python/SelectorTools.py to add it.")
        self.selector_name = selector_map[analysis]
        self.addSumweights = True
        self.ntupleType = "NanoAOD"
        self.numCores = 1
        self.channels = ["Inclusive"]
        self.outfile_name = "temp.root"

    # Needed to parallelize class member function, see
    # https://stackoverflow.com/questions/1816958/cant-pickle-type-instancemethod-when-using-multiprocessing-pool-map
    def __call__(self, args):
        self.processDatasetHelper(args)

    def tempfileName(self, dataset):
        return "temp_%s_%s" % (dataset, self.outfile_name)

    def setChannels(self, channels):
        self.channels = channels

    def setOutputfile(self, outfile_name):
        self.outfile_name = outfile_name
        self.outfile = ROOT.gROOT.FindObject(outfile_name)
        if not self.outfile:
            self.outfile = ROOT.TFile.Open(outfile_name)
        self.current_file = self.outfile

    def addTNamed(self, name, title):
        if not self.inputs:
            raise ValueError("Input list is not defined!")
        named = self.inputs.FindObject(name)
        if named:
            named.SetTitle(title)
        else:
            named = ROOT.TNamed(name, title)
            self.inputs.Add(named)

    def setInputs(self, inputs):
        self.inputs = ROOT.TList()
        for inp in inputs:
            self.inputs.Add(inp)
        self.addTNamed("ntupleType", self.ntupleType)
        self.addTNamed("selection", self.selection)
        
    def setNtupeType(self, ntupleType):
        self.ntupleType = ntupleType
        self.addTNamed("ntupleType", self.ntupleType)

    def setNumCores(self, numCores):
        self.numCores = numCores

    def applySelector(self, datalist):
        for chan in self.channels:
            self.addTNamed("channel", chan)
            print "INFO: Processing channel %s" % chan
            datasets = ConfigureJobs.getListOfFiles(datalist, self.input_tier)
            if self.numCores > 1:
                self.processParallelByDataset(datasets, chan)
            else: 
                for dataset in datasets:
                    self.processDataset(dataset, chan)

    def processDataset(self, dataset, chan):
        print "Processing dataset %s" % dataset
        select = getattr(ROOT, self.selector_name)()
        select.SetInputList(self.inputs)
        # Intended for running over single file, use the format name:file
        specified_name = dataset.split("@")
        if len(specified_name) == 2:
            self.addTNamed("name", specified_name[0])
            dataset = specified_name[1]
        else:
            self.addTNamed("name", dataset)
        try:
            file_path = ConfigureJobs.getInputFilesPath(dataset, 
                self.input_tier, self.analysis)
            # Only add for one channel
            addSumweights = self.addSumweights and self.channels.index(chan) == 0 and "data" not in dataset
            if addSumweights:
                ROOT.gROOT.cd()
                sumweights_hist = ROOT.TH1D("sumweights", "sumweights", 100, 0, 100)
            self.processLocalFiles(select, file_path, addSumweights, chan)
        except ValueError as e:
            print e
            return
        output_list = select.GetOutputList()
        name = self.inputs.FindObject("name").GetTitle()
        dataset_list = output_list.FindObject(name)
        if not dataset_list or dataset_list.ClassName() != "TList":
            print "WARNING: No output found for dataset %s" % dataset
            dataset_list = output_list.FindObject("Unknown")
            if dataset_list and dataset_list.ClassName() == "TList":
                print 'WARNING: Falling back to dataset "Unknown"'
            else:
                print 'WARNING: Skipping dataset %s' % dataset
                return False
        if addSumweights:
            dataset_list.Add(ROOT.gROOT.FindObject("sumweights"))
        if self.numCores > 1:
            self.outfile.Close()
            chanNum = self.channels.index(chan)
            self.current_file = ROOT.TFile.Open(self.tempfileName(dataset), "recreate" if chanNum == 0 else "update")
        OutputTools.writeOutputListItem(dataset_list, self.current_file)
        output_list.Delete()
        if self.current_file != self.outfile:
            self.current_file.Close()
        return True

    def getFileNames(self, file_path):
        xrootd = "/store/user" in file_path.split("/hdfs/")[0][:12]
        if not (xrootd or os.path.isfile(file_path) or os.path.isdir(file_path.rsplit("/", 1)[0].rstrip("/*"))):
            raise ValueError("Invalid path! Skipping dataset. Path was %s" 
                % file_path)

        # Assuming these are user files on HDFS, otherwise it won't work
        filenames =  glob.glob(file_path) if not xrootd else \
                ConfigureJobs.getListOfHDFSFiles(file_path)
        filenames = ['root://cmsxrootd.hep.wisc.edu/' + f if "/store/user" in f[0:12] else f for f in filenames]
        return filenames

    def getTreeName(self, chan):
        # TODO: Fix this! This is an extremely ineffient way to separate the eemm and mmee
        # since it involves reading the file an extra time
        channel = chan if chan != "mmee" else "eemm"
        return "Events" if self.ntupleType == "NanoAOD" else ("%s/ntuple" % channel)

    def processParallelByDataset(self, datasets, chan):
        numCores = min(self.numCores, len(datasets))
        p = multiprocessing.Pool(processes=self.numCores)
        p.map(self, [[dataset, chan] for dataset in datasets])
        # Store arrays in temp files, since it can get way too big to keep around in memory
        tempfiles = [self.tempfileName(d) for d in datasets] 
        tempfiles = filter(os.path.isfile, tempfiles)
        rval = subprocess.call(["hadd", "-f", self.outfile_name] + tempfiles)
        if rval == 0:
            map(os.remove, tempfiles)
        else:
            raise RuntimeError("Failed to collect data from parallel run")

    # Pool.map can only take in one argument, so expand the array
    def processDatasetHelper(self, args):
        self.processDataset(*args)

    def processLocalFiles(self, selector, file_path, addSumweights, chan,):
        filenames = self.getFileNames(file_path)
        for i, filename in enumerate(filenames):
            self.processFile(selector, filename, addSumweights, chan, i+1)

    def processFile(self, selector, filename, addSumweights, chan, filenum=1):
        rtfile = ROOT.TFile.Open(filename)
        tree_name = self.getTreeName(chan)
        tree = rtfile.Get(tree_name)
        if not tree:
            raise ValueError(("tree %s not found for file %s. " \
                    "Probably the file is corrupted") % (tree_name, filename)
            )

        tree.Process(selector, "")
        if addSumweights:
            self.fillSumweightsHist(rtfile, filenum)
        rtfile.Close()

    # You can use filenum to index the files and sum separately, but it's not necessary
    def fillSumweightsHist(self, rtfile, filenum=1):
        if self.ntupleType == "NanoAOD":
            sumweights_branch = "genEventSumw"
            meta_tree_name = "Runs"
        else:
            sumweights_branch = "summedWeights"
            meta_tree_name = "metaInfo/metaInfo"
        meta_tree = rtfile.Get(meta_tree_name)
        ROOT.gROOT.cd()
        sumweights_hist = ROOT.gROOT.FindObject("sumweights")
        tmplabel = sumweights_hist.GetName()+"_i"
        tmpweights_hist = sumweights_hist.Clone(tmplabel)
        meta_tree.Draw("%i>>%s" % (filenum, tmplabel), sumweights_branch)
        sumweights_hist.Add(tmpweights_hist)
