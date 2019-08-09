import logging
import HistTools
import OutputTools
import ConfigureJobs
from prettytable import PrettyTable
import ROOT
import sys

class CombineCardTools(object):
    def __init__(self):
        self.fitVariable = ""
        self.processes = []
        self.yields = {}
        self.histData = {}
        self.managerPath = ""
        self.outputFile = ""
        self.channels = []
        self.variations = {}
        self.rebin = None
        self.isMC = True
        self.isUnrolledFit = False
        self.lumi = 1

    def setProcesses(self, processes):
        self.processes = processes

    def setFitVariable(self, variable):
        self.fitVariable = variable

    def setVariations(self, variations):
        if not self.processes:
            raise ValueError("No processes defined, can't set variations")
        for process in self.processes:
            self.setVariationsByProcess(process, variations)

    def getVariations(self):
        return self.variations

    def getVariationsForProcess(self, process):
        if process not in self.variations.keys():
            raise ValueError("Variations not defined for process %s" % process)
        return self.variations[process]

    def setVariationsByProcess(self, process, variations):
        if process not in self.processes:
            self.processes.append(process)
        if "Up" not in variations and "Down" not in variations:
            variations = [x+y for x in variations for y in ["Up", "Down"]]
        self.variations[process] = variations

    def weightHistName(self, channel):
        variable = self.fitVarible.replace("unrolled", "2D") if self.isUnrolledFit else self.fitVariable
        return "_".join([variable, "lheWeights", channel])

    def setLumi(self, lumi):
        self.lumi = lumi

    def setOutputFolder(self, outputFolder):
        self.outputFolder = outputFolder

    def getRootFile(self, rtfile, mode=None):
        if type(rtfile) == str:
            if mode:
                return ROOT.TFile.Open(rtfile, mode)
            else:
                return ROOT.TFile.Open(rtfile)
        return rtfile

    def setOutputFile(self, outputFile):
        self.outputFile = self.getRootFile(outputFile, "RECREATE")

    def setInputFile(self, inputFile):
        self.inputFile = self.getRootFile(inputFile)

    def setChannels(self, channels):
        self.channels = channels

    def processHists(self, processName):
        return self.histData[processName] 

    def combineChannels(self, group, central=True):
        variations = self.variations[group.GetName()][:]
        if central:
            variations.append("")
        for var in variations:
            varname =self.fitVariable.replace("_", "-")
            varname = varname.replace("--", "__")
            name = varname if var is "" else "_".join([self.fitVariable, var])
            hist_name = name + "_" + self.channels[0]
            hist = group.FindObject(hist_name)
            if not hist:
                logging.warning("Failed to find hist %s in group %s. Skipping" % (hist_name, group.GetName()))
                continue
            hist = hist.Clone(name)
            ROOT.SetOwnership(hist, False)
            group.Add(hist) 
            for chan in self.channels[1:]:
                chan_hist = group.FindObject(name + "_" + chan)
            hist.Add(chan_hist)

    def listOfPlotsByProcess(self, processName, addTheory):
        if self.fitVariable == "":
            raise ValueError("Must define variable name before defining plots")
        plots = ["_".join([self.fitVariable, chan]) for chan in self.channels]
        variations = self.getVariationsForProcess(processName)
        plots += ["_".join([self.fitVariable, var, c]) for var in variations for c in self.channels]
        if addTheory:
            plots += [self.weightHistName(c) for c in self.channels]
        return plots

    # processName needs to match a PlotGroup defined in AnalysisDatasetManager
    def loadHistsForProcess(self, processName, addTheory, scaleNorm=1):
        plotsToRead = self.listOfPlotsByProcess(processName, addTheory)

        manager_path = ConfigureJobs.getManagerPath() 
        sys.path.append("/".join([manager_path, "AnalysisDatasetManager",
            "Utilities/python"]))

        from ConfigHistFactory import ConfigHistFactory
        config_factory = ConfigHistFactory(
            "%s/AnalysisDatasetManager" % manager_path,
            "WZxsec2016/VBSselection",
        )

        group = HistTools.makeCompositeHists(self.inputFile, processName, 
                    ConfigureJobs.getListOfFilesWithXSec(
                        config_factory.getPlotGroupMembers(processName), manager_path), 
                self.lumi, plotsToRead, rebin=self.rebin)

        #TODO:Make optional
        for chan in self.channels:
            histName = "_".join([self.fitVariable, chan]) if chan != "all" else self.fitVariable
            hist = group.FindObject(histName)
            print "Bonjour", histName, hist
            self.yields[chan] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.0001
            if addTheory:
                weightHist = group.FindObject(self.weightHistName(chan))
                if not weightHist:
                    logging.warning("Failed to find %s. Skipping" % self.weightHistName(chan))
                    continue
                scaleHists = HistTools.getScaleHists(weightHist, processName, self.rebin)
                group.extend(scaleHists)
            #TODO: Make optional
            #for hist in group:
            #    HistTools.addOverflowAndUnderflow(hist,underflow=False)
        self.combineChannels(group)
        map(HistTools.removeZeros, group)
        self.histData[processName] = group

    # It's best to call this function for process, otherwise you can end up
    # storing many large histograms in memory
    def writeProcessHistsToOutput(self, processName):
        if processName not in self.histData.keys() or not self.histData[processName]:
            raise ValueError("Hists for process %s not found" % processName)
        processHists = self.histData[processName]
        print "processHists are", processHists
        OutputTools.writeOutputListItem(processHists, self.outputFile)
        processHists.Delete()
        
    #def writeCards()
