from python import CombineCardTools
from python import ConfigureJobs
import sys
import ROOT
import logging
import array
import argparse

ROOT.gROOT.SetBatch(True)

parser = argparse.ArgumentParser()
parser.add_argument("--debug", action='store_true',
    help="Print debug info")
parser.add_argument("--mc2hes", action='store_true',
    help="Convert MC errors to hessian")
parser.add_argument("-c", "--central", type=str, default="wlnu_jetbinned_nlo_cp5",
    help="Sample to use as central value")
parser.add_argument("-d", "--data", type=str, default="wlnu_nlo",
    help="Sample to use as dummy data")
parser.add_argument("-a", "--append", type=str, default="",
    help="Append to output folder name")
parser.add_argument("-b", "--fitvar", type=str, default="ptWmet",
    help="Variable to use in the fit")
parser.add_argument("-r", "--rebin", 
                    type=lambda x : [] if "," not in x else [float(i.strip()) for i in x.split(',')],
                    default=[i for i in range(0, 120, 2)], help="Rebin array: "
                    "values (bin edges) separated by commas.")
args = parser.parse_args()

logging.basicConfig(level=(logging.DEBUG if args.debug else logging.INFO))

cardtool = CombineCardTools.CombineCardTools()

manager_path = ConfigureJobs.getManagerPath() 
sys.path.append("/".join([manager_path, "AnalysisDatasetManager",
    "Utilities/python"]))

from ConfigHistFactory import ConfigHistFactory
config_factory = ConfigHistFactory(
    "%s/AnalysisDatasetManager" % manager_path,
    "WGen/NanoAOD",
)

#plot_groups = ["wlnu_lo", "wlnu_lo_cp5", "wlnu_nlo", "wlnu_jetbinned_nlo", "wlnu_jetbinned_nlo_cp5", ]
plot_groups = ["wlnu_nlo", ]
plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in plot_groups}

xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])

channels = ["ep", "en", "mp", "mn"]
if args.rebin:
    rebin = array.array('d', args.rebin)
    cardtool.setRebin(rebin)
cardtool.setFitVariable(args.fitvar)
cardtool.setProcesses(plotGroupsMap)
cardtool.setChannels(channels)
cardtool.setCrosSectionMap(xsecs)
cardtool.setVariations([])
folder_name = "_".join([args.fitvar,args.append]) if args.append != "" else args.append
cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/WGen/%s" % folder_name)

#cardtool.setLumi(35.9)
cardtool.setLumi(0.2)
#cardtool.setInputFile("/eos/user/k/kelong/HistFiles/WGen/combinedJetBinned.root")
cardtool.setInputFile("/afs/cern.ch/user/k/kelong/work/WZAnalysis/CMSSW_10_6_0_patch1/src/Analysis/VVAnalysis/test.root")
cardtool.setOutputFile("WGenCombineInput.root")
cardtool.setCombineChannels({"all" : channels, "e" : ["ep", "en"], "m" : ["mp", "mn"]})
for process in plot_groups:
    #Turn this back on when the theory uncertainties are added
    if process not in ["nonprompt", "data"]: #and False
        cardtool.addTheoryVar(process, 'scale', range(1, 10), exclude=[6, 7], central=4)
        if "cp5" not in process and args.mc2hes:
            # Exclude alpha_s variations
            pdf_entries = [5] + range(112, 172) 
            cardtool.addTheoryVar(process, "pdf_hessian", pdf_entries, central=0)
        else:
            pdf_entries = [5] + (range(10, 110) if "cp5" not in process else range(10, 40))
            cardtool.addTheoryVar(process, 'pdf_mc' if "cp5" not in process else "pdf_hessian", pdf_entries, central=0)
    cardtool.loadHistsForProcess(process)
    cardtool.writeProcessHistsToOutput(process)

nuissance_map = {"e" : 34, "m" : 34 }
for chan in ["e", "m"]:
    cardtool.setTemplateFileName("Templates/CombineCards/VGen/WGen_template_{channel}.txt")
    logging.info("Writting cards for channel %s" % chan)
    cardtool.writeCards(chan, nuissance_map[chan], 
        extraArgs={"data_name" : args.data, 
            "w_sample" : args.central, "w_yield" : "yield:%s" % args.central, 
        }
    )

