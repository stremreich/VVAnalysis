from python import CombineCardTools
from python import ConfigureJobs
import sys
import ROOT
import logging

cardtool = CombineCardTools.CombineCardTools()

manager_path = ConfigureJobs.getManagerPath() 
sys.path.append("/".join([manager_path, "AnalysisDatasetManager",
    "Utilities/python"]))

from ConfigHistFactory import ConfigHistFactory
config_factory = ConfigHistFactory(
    "%s/AnalysisDatasetManager" % manager_path,
    "ZZRunII",
)

plot_groups = ["HZZ_signal","qqZZ_powheg","ggZZ", "VVV", "nonprompt", "data"]
plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in plot_groups}

xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])

year = 2018
channels = ["eeee", "eemm", "mmee", "mmmm"]
nuissance_map = {"eeee" : 9, "eemm" : 10, "mmee" : 10, "mmmm" : 8, "all" : 6}

cardtool.setLumi(41.5)
cardtool.setFitVariable("ZZPt")
cardtool.setFitVariableAppend("nonprompt", "Fakes")
cardtool.setProcesses(plotGroupsMap)
cardtool.setChannels(channels)
cardtool.setInputFile("/eos/user/k/kelong/HistFiles/ZZ/Hists16Mar2019-ZZ4l2018Full.root")
cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/ZZ")
cardtool.setOutputFile("ZZCombineInput_{year}.root".format(year=year))
cardtool.setCrosSectionMap(xsecs)
cardtool.setVariations(["CMS_eff_e", "CMS_RecoEff_e", "CMS_eff_m", "CMS_pileup"],
                        exclude=["nonprompt", "data"])

for process in plot_groups:
    #Turn this back on when the theory uncertainties are added
    addTheory = process not in ["nonprompt"] and False
    cardtool.loadHistsForProcess(process, addTheory)
    cardtool.writeProcessHistsToOutput(process)

for chan in channels + ["all"]:
    cardtool.setTemplateFileName("Templates/CombineCards/ZZSelection/ZZ_template{year}_{channel}.txt")
    logging.info("Writting cards for channel %s" % chan)
    cardtool.writeCards(chan, nuissance_map[chan], year=year)

