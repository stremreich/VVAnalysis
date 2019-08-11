# coding: utf-8
from Utilities.python import CombineCardTools
from Utilities.python import ConfigureJobs
import sys
import ROOT
cardtool = CombineCardTools.CombineCardTools()


manager_path = ConfigureJobs.getManagerPath() 
sys.path.append("/".join([manager_path, "AnalysisDatasetManager",
    "Utilities/python"]))

from ConfigHistFactory import ConfigHistFactory
config_factory = ConfigHistFactory(
    "%s/AnalysisDatasetManager" % manager_path,
    "WZxsec2016/VBSselection",
)

plot_groups = ["EW_WZjj", "QCD_WZjj", "vv", "top_ewk", "zg", "nonprompt"]
plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in plot_groups}

xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])
print xsecs

cardtool.setLumi(35.9)
cardtool.setFitVariable("MTWZ")
cardtool.setChannels(["eee", "eem", "emm", "mmm"])
cardtool.setInputFile("/eos/home-k/kelong/WZAnalysisData/HistFiles/VBSselection_Tight_Full-02Oct2018_SingleMuPrefireEff.root")
cardtool.setOutputFile("test.root")
cardtool.setProcesses(plotGroupsMap)
cardtool.setCrosSectionMap(xsecs)
cardtool.setVariations(["CMS_scale_j", "CMS_res_j", "CMS_eff_m", "CMS_scale_m", "CMS_eff_e", "CMS_scale_e", "CMS_pileup"],
                        exclude=["nonprompt"])
cardtool.setVariationsByProcess("nonprompt", ["CMS_scale_j", "CMS_res_j",])
for process in plot_groups:
    addTheory = process not in ["nonprompt", "zg"]
    print "Plots from process %s are %s" % (process, cardtool.listOfHistsByProcess(process, addTheory))
    cardtool.loadHistsForProcess(process, addTheory)
    cardtool.writeProcessHistsToOutput(process)

cardtool.setTemplateFileName("Templates/CombineCards/VBSselection/WZjj_EWK_template_{channel}.txt")
cardtool.writeCards("eee", 10, year="2016")
