# coding: utf-8
from Utilities.python import CombineCardTools
import ROOT
cardtool = CombineCardTools.CombineCardTools()
cardtool.setFitVariable("MTWZ")
cardtool.setChannels(["eee", "eem", "emm", "mmm"])
cardtool.setInputFile("/eos/home-k/kelong/WZAnalysisData/HistFiles/VBSselection_Tight_Full-02Oct2018_SingleMuPrefireEff.root")
cardtool.setOutputFile("test.root")
cardtool.setProcesses(["wz_powheg", "dy_lo", "vv_powheg"])
cardtool.setVariations(["CMS_scale_j", "CMS_res_j", "CMS_eff_m", "CMS_scale_m", "CMS_eff_e", "CMS_scale_e", "CMS_pileup"])
print "All variations", cardtool.getVariations()
print "For WZ", cardtool.listOfPlotsByProcess("wz_powheg", True)
cardtool.loadHistsForProcess("wz_powheg", True, 1)
cardtool.writeProcessHistsToOutput("wz_powheg")
