#include "Analysis/VVAnalysis/interface/LowPileupZSelector.h"
#include <TStyle.h>
#include <regex>

void LowPileupZSelector::Init(TTree *tree)
{
    allChannels_ = {"ee", "mm"};
    // Add CutFlow for Unknown to understand when channels aren't categorized
    hists1D_ = {"CutFlow", "ZMass", "ZEta", "yZ", "ZPt", "ptl1", "etal1", "ptl2", "etal2",
        "ptj1", "ptj2", "ptj3", "etaj1", "etaj2", "etaj3", "phij1", "phij2", "phij3", "nJets",
        "MET",};

    b.SetTree(tree);
    SelectorBase::Init(tree);
}

void LowPileupZSelector::SetBranchesBacon() {
    b.CleanUp();
    b.SetBranch("zCand", zCand);
    b.SetBranch("lep1", lep1);
    b.SetBranch("lep2", lep2);
    b.SetBranch("genVPt", genVPt);
    b.SetBranch("genVPhi", genVPhi);
    b.SetBranch("genVy", genVy);
    b.SetBranch("genVMass", genVMass);
    b.SetBranch("met", met);

    if (isMC_) {
        b.SetBranch("genWeight", genWeight);
        b.SetBranch("PUWeight", PUWeight);
    }
}

void LowPileupZSelector::LoadBranchesBacon(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    b.SetEntry(entry);

    if (isMC_) {
        weight = genWeight*PUWeight;
    }
    SetComposite();
}

void LowPileupZSelector::SetComposite() {
    zCand = lep1+lep2;
}

void LowPileupZSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    SafeHistFill(histMap1D_, getHistName("mZ", variation.second), zCand.M(), weight);
    SafeHistFill(histMap1D_, getHistName("ptZ", variation.second), zCand.Pt(), weight);
    SafeHistFill(histMap1D_, getHistName("yZ", variation.second), zCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl1", variation.second), lep1.Pt(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl2", variation.second), lep2.Pt(), weight);
    SafeHistFill(histMap1D_, getHistName("etal1", variation.second), lep1.Eta(), weight);
    SafeHistFill(histMap1D_, getHistName("etal2", variation.second), lep2.Eta(), weight);
    SafeHistFill(histMap1D_, getHistName("MET", variation.second), met, weight);
}

void LowPileupZSelector::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}

