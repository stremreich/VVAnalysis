from python import CombineCardTools
from python import ConfigureJobs
import sys
import ROOT
import logging
import array

cardtool = CombineCardTools.CombineCardTools()

manager_path = ConfigureJobs.getManagerPath() 
manager_name = ConfigureJobs.getManagerName() 
sys.path.append("/".join([manager_path, "AnalysisDatasetManager",
    "Utilities/python"]))

from ConfigHistFactory import ConfigHistFactory
config_factory = ConfigHistFactory(
    "%s/%s" % (manager_path, manager_name),
    "ZZ4l2016/LooseLeptons",
)

plot_groups = ["HZZ_signal","qqZZ_powheg","ggZZ", "VVV", "data", "nonprompt",] 
plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in plot_groups}

xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])

lumiMap = {"2016" : 35.9, "2017" : 41.5, "2018" : 59.74}
fileMap = { "2017" : "/eos/user/k/kelong/HistFiles/ZZ/Hists13Aug2019-ZZ4l2017Full.root",
    "2018" : "/eos/user/k/kelong/HistFiles/ZZ/Hists13Aug2019-ZZ4l2018Full.root",
    "2016" : "/eos/user/k/kelong/HistFiles/ZZ/Hists29Aug2019-ZZ4l2016.root",
}
channels = ["eeee", "eemm", "mmee", "mmmm"]
nuissance_map = {"eeee" : 9, "eemm" : 10, "mmee" : 10, "mmmm" : 8, "all" : 6}
#fitvar = "ZZPt"
#rebin = array.array('d', [0.0,50.0,100.0,150.0,200.0,250.0,300.0,350.0,400.0])
fitvar = "Mass"
rebin = array.array('d', [100.0,200.0,250.0,300.0,350.0,400.0,500.0,600.0,800.0,1000.0,1200.0])
cardtool.setFitVariable(fitvar)
cardtool.setRebin(rebin)
#cardtool.setFitVariableAppend("nonprompt", "Fakes")
cardtool.setProcesses(plotGroupsMap)
cardtool.setChannels(channels)
cardtool.setCrosSectionMap(xsecs)
cardtool.setVariations(["CMS_eff_e", "CMS_RecoEff_e", "CMS_eff_m", ],#"CMS_pileup"],
                        exclude=["nonprompt", "data"])
cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/ZZ/%s2016Fit" % fitvar)
#cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/ZZ/%s2017Fit" % fitvar)

for year in ["2016"]:#fileMap.keys():
    cardtool.setLumi(lumiMap[year])
    cardtool.setInputFile(fileMap[year])
    print fileMap[year], lumiMap[year] 
    cardtool.setOutputFile("ZZCombineInput_{year}.root".format(year=year))
    #cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/ZZ/%s%sFit" % (fitvar, year))
    for process in plot_groups:
        #Turn this back on when the theory uncertainties are added
        addTheory = process not in ["nonprompt"] and False
        cardtool.loadHistsForProcess(process, addTheory)
        cardtool.writeProcessHistsToOutput(process)

    for chan in channels + ["all"]:
        cardtool.setTemplateFileName("Templates/CombineCards/ZZSelection/ZZ_template{year}_{channel}.txt")
        logging.info("Writting cards for channel %s" % chan)
        cardtool.writeCards(chan, nuissance_map[chan], year=year)

