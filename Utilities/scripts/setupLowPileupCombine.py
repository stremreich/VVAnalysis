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
parser.add_argument("-u", '--unbinnedSignal', action='store_true',
    help="Just use one signalStrength")
parser.add_argument("-a", "--append", type=str, default="",
    help="Append to output folder name")
parser.add_argument("-r", "--rebin", 
                    type=lambda x : [] if "," not in x else [float(i.strip()) for i in x.split(',')],
                    default=[i for i in range(0, 100, 10)], help="Rebin array: "
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
    "LowPileupW/NanoAOD",
)

plot_groups = ["data", "nonprompt", "ewk", "top", "wmv_0j_nlo", "wmv_1j_nlo", "wmv_2j_nlo", "wmv_jetbinned_nlo_pt0to10", "wmv_jetbinned_nlo_pt10to20", "wmv_jetbinned_nlo_pt20to30", "wmv_jetbinned_nlo_pt30to40", "wmv_jetbinned_nlo_pt40to50", "wmv_jetbinned_nlo_pt50to60", "wmv_jetbinned_nlo_pt60to70", "wmv_jetbinned_nlo_pt70to80", "wmv_jetbinned_nlo_pt80to90", "wmv_jetbinned_nlo_pt90to100", "wmv_jetbinned_nlo_pt100", "wmv_jetbinned_nlo",]
plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in plot_groups}

xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])

channels = ["mp", "mn"]
if args.rebin:
    rebin = array.array('d', args.rebin)
    cardtool.setRebin(rebin)
#fitvar = "ptW"
fitvar = "pfMet"
cardtool.setFitVariable(fitvar)
cardtool.setProcesses(plotGroupsMap)
cardtool.setChannels(channels)
cardtool.setCrosSectionMap(xsecs)
cardtool.setVariations([])
folder_name = "_".join([fitvar,args.append]) if args.append != "" else fitvar
cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/LowPileup/%s" % folder_name)
cardtool.setFitVariableAppend("nonprompt", "Fakes")

#cardtool.setLumi(35.9)
cardtool.setLumi(0.199)
#cardtool.setInputFile("/eos/user/k/kelong/HistFiles/WGen/combinedJetBinned.root")
cardtool.setInputFile("/afs/cern.ch/user/k/kelong/work/WZAnalysis/CMSSW_10_6_0_patch1/src/Analysis/VVAnalysis/testW.root")
cardtool.setOutputFile("WGenCombineInput.root")
#cardtool.setCombineChannels({"all" : channels, "m" : ["mp", "mn"]})
cardtool.setCombineChannels({"all" : channels, "m" : ["mp", "mn"]})
for process in plot_groups:
    #Turn this back on when the theory uncertainties are added
    if process not in ["nonprompt", "data"] and False:
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

nuissance_map = {"e" : 2, "m" : 2}
template_name = "W" if args.unbinnedSignal else "WPtBinned"
for chan in ["m"]:
    cardtool.setTemplateFileName("Templates/CombineCards/LowPileup/W_template_{channel}.txt")
    logging.info("Writting cards for channel %s" % chan)
    cardtool.writeCards(chan, nuissance_map[chan],)


