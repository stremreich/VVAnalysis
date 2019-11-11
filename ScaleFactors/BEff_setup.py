from ROOT import TFile, gDirectory, TH2D, TList
import numpy as np
import array
import time

ptBins = np.array([10.0, 20, 30, 40, 50, 70, 100, 150, 200, 250, 300, 400, 500, 670, 1000, 1250, 1500, 2000, 3000])
etaBins = np.array([-2.5, -1.5, 0, 1.5, 2.5])

realptBin = [2, 3, 4, 5, 6, 8, 11, 16, 21, 26, 31, 41, 51, 68, 101, 126, 151, 201, 301]
realetaBin = [1, 3, 6, 9, 11]
bmap = {"Beff_b": ["Beff_b_btag_all", "Beff_b_all"],
        "Beff_j": ["Beff_j_btag_all", "Beff_j_all"],}

infileName="beff_test.root"
infile = TFile(infileName)

outfileName="BEff_0926.root"
outfile = TFile(outfileName, "RECREATE")

for key in infile.GetListOfKeys():
    
    outfile.mkdir(key.GetTitle())
    
    
    for newName, oldNames in bmap.iteritems():
        print newName, oldNames
        infile.cd(key.GetTitle())
        oldHist_pass = gDirectory.Get(oldNames[0])
        oldHist_all = gDirectory.Get(oldNames[1])        
        newHist = TH2D(newName, newName, len(ptBins) - 1, ptBins, len(etaBins) - 1, etaBins)
        for i in xrange(len(ptBins) - 1):
            for j in xrange(len(etaBins) - 1):
                xlow = realptBin[i]
                xhigh = realptBin[i + 1] - 1
                ylow = realetaBin[j]
                yhigh = realetaBin[j + 1] - 1
                if oldHist_all.Integral(xlow, xhigh, ylow, yhigh) == 0:
                    ratio = 0
                else:
                    print oldHist_pass.Integral(xlow, xhigh, ylow, yhigh), oldHist_all.Integral(xlow, xhigh, ylow, yhigh)
                    ratio = oldHist_pass.Integral(xlow, xhigh, ylow, yhigh) / oldHist_all.Integral(xlow, xhigh, ylow, yhigh)
                #print ratio
                newHist.SetBinContent(i + 1, j + 1, ratio)
        # exit(0)
        outfile.cd(key.GetTitle())
        newHist.Write()
        
    
    
