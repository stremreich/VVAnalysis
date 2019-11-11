from python import CombineCardTools
from python import ConfigureJobs
import sys
import ROOT
import logging
import array

#logging.basicConfig(level=logging.DEBUG)

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

#doaTGC = False
doaTGC = True
plot_groups = ["HZZ_signal","qqZZ_powheg","ggZZ", "zzjj4l_ewk", "VVV", "data", "nonprompt", ] 
if doaTGC:
    plot_groups += ["zz_atgc_f4_fg_0_fz_0p0015"]
plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in plot_groups}

xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])

lumiMap = {"2016" : 35.9, "2017" : 41.5, "2018" : 59.74}
fileMap = { "2016" : "/eos/user/k/kelong/HistFiles/ZZ/Hists02Sep2019-ZZ4l2016.root",
    "2017" : "/eos/user/k/kelong/HistFiles/ZZ/Hists02Sep2019-ZZ4l2017.root",
    "2018" : "/eos/user/k/kelong/HistFiles/ZZ/Hists02Sep2019-ZZ4l2018.root",
}
channels = ["eeee", "eemm", "mmee", "mmmm"]
nuissance_map = {"eeee" : 18, "eemm" : 19, "mmee" : 19, "mmmm" : 17, "all" : 15}
#fitvar = "ZZPt"
#rebin = array.array('d', [0.0,50.0,100.0,150.0,200.0,250.0,300.0,350.0,400.0])
fitvar = "Mass"
rebin = array.array('d', [100.0,200.0,250.0,300.0,350.0,400.0,500.0,600.0,800.0,1000.0,1200.0])
cardtool.setRebin(rebin)
#fitvar = "yield"
cardtool.setFitVariable(fitvar)
cardtool.setFitVariableAppend("nonprompt", "Fakes")
cardtool.setProcesses(plotGroupsMap)
cardtool.setChannels(channels)
cardtool.setCrosSectionMap(xsecs)
cardtool.setVariations(["CMS_eff_e", "CMS_RecoEff_e", "CMS_eff_m", ],#"CMS_pileup"],
                        exclude=["nonprompt", "data"])
#cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/ZZ/%s2016Fit" % fitvar)
folderName = ("%sFitFullRunII%s" % (fitvar, "aTGC" if doaTGC else ""))
cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/ZZ/%s" % folderName)

for year in fileMap.keys():
    cardtool.setLumi(lumiMap[year])
    cardtool.setInputFile(fileMap[year])
    print fileMap[year], lumiMap[year] 
    cardtool.setOutputFile("ZZCombineInput_{year}.root".format(year=year))
    #cardtool.setOutputFolder("/eos/user/k/kelong/CombineStudies/ZZ/%s%sFit" % (fitvar, year))
    for process in plot_groups:
        if process not in ["nonprompt", "data"] and "atgc" not in process: 
            cardtool.addTheoryVar(process, 'scale', range(1, 10), exclude=[6, 8], central=4)
            cardtool.addTheoryVar(process, 'pdf_hessian' if year != "2016" else 'pdf_mc', [4]+[i for i in range(10, 111)], central=0)
        cardtool.loadHistsForProcess(process)
        cardtool.writeProcessHistsToOutput(process)

    for chan in channels + ["all"]:
        cardname = "ZZ%s_template{year}_{channel}.txt" % ("aTGC" if doaTGC else "")
        cardtool.setTemplateFileName("Templates/CombineCards/ZZSelection/"+ cardname)
        logging.info("Writting cards for channel %s" % chan)
        cardtool.writeCards(chan, nuissance_map[chan], year=year)

