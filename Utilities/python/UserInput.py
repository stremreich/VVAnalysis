import json
import copy
import glob
import argparse
import datetime
from collections import OrderedDict
import ConfigureJobs
import sys
import ROOT
import imp

def getDefaultParser():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--selection", type=str,
                        required=True, help="Name of selection to make, "
                        " as defined in Cuts/<analysis>/<selection>.json")
    parser.add_argument("-v", "--version", type=str,
                        required=False, default="1",
                        help="Version number, appended to name")
    parser.add_argument("-a", "--analysis", type=str,
                        required=False, default="WZxsec2016",
                        help="Analysis name, used in selecting the cut json")
    parser.add_argument("-f", "--filenames", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["WZxsec2016"], help="List of input file names, "
                        "as defined in AnalysisDatasetManager, separated "
                        "by commas")
    return parser

def readAllInfo(file_path):
    info = {}
    for info_file in glob.glob(file_path):
        if "__init__" in info_file:
            continue
        if ".py" not in info_file[-3:] and ".json" not in info_file[-5:]:
            continue
        info.update(readInfo(info_file))
    return info

def readInfo(file_path):
    info = {}
    if ".py" in file_path[-3:]:
        file_info = imp.load_source("info_file", file_path)
        info = file_info.info
    elif ".json" in file_path[-5]:
        info = readJson(file_path)
    return info

def readJson(json_file_name):
    json_info = {}
    with open(json_file_name) as json_file:
        try:
            json_info = json.load(json_file)
        except ValueError as err:
            print "Error reading JSON file %s. The error message was:" % json_file_name 
            print(err)
    return json_info

    
# Depends on AnalysisDatasetManagerModule
def getHistInfo(analysis, input_hists, noConfig=False):
    if noConfig:
        print "INFO: assuming histogram information is specified in selector"
        return (input_hists, [])

    manager_path = ConfigureJobs.getManagerPath()
    ConfigHistTools = imp.load_source("ConfigHistTools", 
        "/".join([manager_path, "AnalysisDatasetManager/Utilities/python/ConfigHistTools.py"]))
    # For histograms produced with some postprocessing on the hist file
    excludedHistPatterns = ["wCR", "unrolled", "YieldByChannel"]
    config_hists = ConfigHistTools.getAllHistNames(manager_path, analysis) \
        if "all" in input_hists else input_hists

    hists = filter(lambda x : all(y not in x for y in excludedHistPatterns), config_hists)
    hist_inputs = [getHistExpr(hists, analysis)]

    return hists, hist_inputs

def getHistExpr(hist_names, selection):
    manager_path = ConfigureJobs.getManagerPath()
    ConfigHistTools = imp.load_source("ConfigHistTools", 
        "/".join([manager_path, "AnalysisDatasetManager/Utilities/python/ConfigHistTools.py"]))

    info = ROOT.TList()
    info.SetName("histinfo")
    for hist_name in hist_names:
        bin_info = ConfigHistTools.getHistBinInfo(manager_path, selection, hist_name)
        if "TH1" in ConfigHistTools.getHistType(manager_path, selection, hist_name):
            bin_expr = "{nbins}, {xmin}, {xmax}".format(**bin_info)
        else:
            bin_expr = "{nbinsx}, {xmin}, {xmax}, {nbinsy}, {ymin}, {ymax}".format(**bin_info)
        info.Add(ROOT.TNamed(hist_name, " $ ".join([hist_name, bin_expr])))
    return info

