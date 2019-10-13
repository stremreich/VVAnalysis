from python import CombineCardTools
from python import ConfigureJobs
import sys
import ROOT
import logging
import array

ROOT.gROOT.SetBatch(True)
logging.basicConfig(level=logging.DEBUG)

cardtool = CombineCardTools.CombineCardTools()

manager_path = ConfigureJobs.getManagerPath() 
sys.path.append("/".join([manager_path, "AnalysisDatasetManager",
    "Utilities/python"]))

from ConfigHistFactory import ConfigHistFactory
config_factory = ConfigHistFactory(
    "%s/AnalysisDatasetManager" % manager_path,
    "WGen/NanoAOD",
)

#plot_groups = ["wlnu", "wlnu_nlo"]
plot_groups = ["wlnu_nlo"]
plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in plot_groups}

xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])

channels = ["e", "m"]
fitvar = "ptW"
rebin = array.array('d', [i for i in range(20, 120, 5)])
cardtool.setFitVariable(fitvar)
cardtool.setRebin(rebin)
cardtool.setProcesses(plotGroupsMap)
cardtool.setChannels(channels)
cardtool.setCrosSectionMap(xsecs)
cardtool.setVariations([])
cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/WGen/%s" % fitvar)

cardtool.setLumi(35.9)
cardtool.setInputFile("/eos/user/k/kelong/HistFiles/WGen/combined.root")
cardtool.setOutputFile("WGenCombineInput.root")
for process in plot_groups:
    #Turn this back on when the theory uncertainties are added
    if process not in ["nonprompt", "data"]: #and False
        cardtool.addTheoryVar(process, 'scale', range(1, 9), exclude=[6, 7], central=4)
        pdf_entries = [4] + range(9, 109)
        cardtool.addTheoryVar(process, 'pdf_mc', pdf_entries, central=0)
    cardtool.loadHistsForProcess(process)
    cardtool.writeProcessHistsToOutput(process)

nuissance_map = {"e" : 3, "m" : 3 }
for chan in channels: #+ ["all"]:
    cardtool.setTemplateFileName("Templates/CombineCards/VGen/WGen_template_{channel}.txt")
    logging.info("Writting cards for channel %s" % chan)
    cardtool.writeCards(chan, nuissance_map[chan], 
        #extraArgs={"data_name" : "dy_lo", "dy_sample" : "dy_lo"})
        extraArgs={"data_name" : "wlnu_nlo", "w_sample" : "wlnu_nlo"})

