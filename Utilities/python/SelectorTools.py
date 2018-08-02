#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os

def applySelector(filelist, selector_name, selection, 
        rootfile,
        analysis="WZxsec2016", channels=["eee", "eem", "emm", "mmm"], 
        extra_inputs = [],
        addSumweights=True,
        proof=False):
    for i, chan in enumerate(channels):
        inputs = ROOT.TList()
        for inp in extra_inputs:
            inputs.Add(inp)
        for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
            select = getattr(ROOT, selector_name)()
            select.SetInputList(inputs)
            print "Processing channel %s for dataset %s" % (chan, dataset)
            try:
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                    selection, analysis)
                print "extra_inputs: ", extra_inputs
                print "selector_name: ", selector_name
                print "calling processLocalFiles:"
                print "arg1(select): ", select
                print "arg2(file_path): ", file_path
                print "arg3(chan): ", chan
                processLocalFiles(select, file_path, chan)
                print "check 2"
            except ValueError as e:
                print e
                continue
            output_list = select.GetOutputList()
            dataset_list = output_list.FindObject(dataset)
            if not dataset_list or dataset_list.ClassName() != "TList":
                print "WARNING: No output found for dataset %s" % dataset
                continue
            # Only add for one channel
            if addSumweights and i == 0:
                meta_chain = ROOT.TChain("metaInfo/metaInfo")
                meta_chain.Add(file_path)
                sumweights = ROOT.TH1D("sumweights", "sumweights", 1, 0, 10)
                meta_chain.Draw("1>>sumweights", "summedWeights")
                dataset_list.Add(ROOT.gROOT.FindObject("sumweights"))
            OutputTools.writeOutputListItem(dataset_list, rootfile)
            print "made it to deletes"
            output_list.Delete()
            ROOT.gROOT.GetList().Delete()
            print "made it past deletes"
        #if chain.GetEntries() != 0 and proof:
        #proof_path = "_".join([analysis, selection+("#/%s/ntuple" % chan)])
        #ROOT.gProof.Process(proof_path, select, "")

def processLocalFiles(selector, file_path, chan):
    if not os.path.isdir(file_path.rsplit("/", 1)[0]):
        raise ValueError("Invalid path! Path was %s" 
            % file_path)
    print "check before loop"
    for filename in glob.glob(file_path):
        print "check inside loop, filename: ", filename
        rtfile = ROOT.TFile(filename)
        print "after ROOT.tfile"
        tree = rtfile.Get("%s/ntuple" % chan)
        print "after trfile.Get(%s/ntuple" % chan, ")"
        if not tree:
            raise ValueError(("tree %s/ntuple not found for file %s. " \
                    "Probably it is corrupted") % (chan, filename)
            )
        
        print "before tree.Process"
        tree.Process(selector, "")
        print "after tree.Process"
