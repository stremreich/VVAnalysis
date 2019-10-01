#include "Analysis/VVAnalysis/interface/LowPileupWSelector.h"
#include <TStyle.h>
#include <regex>

void LowPileupWSelector::Init(TTree *tree)
{
    allChannels_ = {"m"};
    hists1D_ = {"CutFlow", "mV", "yW", "ptW", "ptl", "etal", "pfMet",};

    LowPileupSelector::Init(tree);
}

void LowPileupWSelector::SetBranchesBacon() {
    channel_ = mm;
    channelName_ = "mm";
    lep = nullptr;
    fChain->SetBranchAddress("lep", &lep, &lep_b);
    LowPileupSelector::SetBranchesBacon();
}

void LowPileupWSelector::LoadBranchesBacon(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    lep_b->GetEntry(entry);
    LowPileupSelector::LoadBranchesBacon(entry, variation);
}

void LowPileupWSelector::SetComposite() {
}

void LowPileupWSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    SafeHistFill(histMap1D_, getHistName("mW", variation.second), wCand.M(), weight);
    SafeHistFill(histMap1D_, getHistName("ptW", variation.second), wCand.Pt(), weight);
    SafeHistFill(histMap1D_, getHistName("yW", variation.second), wCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl", variation.second), lep->Pt(), weight);
    SafeHistFill(histMap1D_, getHistName("etal", variation.second), lep->Eta(), weight);
    SafeHistFill(histMap1D_, getHistName("pfMet", variation.second), pfMet, weight);
}

