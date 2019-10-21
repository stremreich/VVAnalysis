#include "Analysis/VVAnalysis/interface/LowPileupWSelector.h"
#include <TStyle.h>
#include <regex>

void LowPileupWSelector::Init(TTree *tree)
{
    allChannels_ = {"mp", "mn"};
    hists1D_ = {"CutFlow", "mW", "yW", "ptW", "ptl", "etal", "pfMet",};

    fReader.SetTree(tree);
    LowPileupSelector::Init(tree);
}

void LowPileupWSelector::SetBranchesBacon() {
    lep = nullptr;
    fChain->SetBranchAddress("lep", &lep, &lep_b);
    LowPileupSelector::SetBranchesBacon();
}

void LowPileupWSelector::LoadBranchesBacon(Long64_t entry, SystPair variation) { 
    lep_b->GetEntry(entry);
    fReader.SetLocalEntry(entry);
    LowPileupSelector::LoadBranchesBacon(entry, variation);
    if (isMC_)
        weight = evtWeight[0];
    if (*charge > 0) {
        channel_ = mp;
        channelName_ = "mp";
    }
    else {
        channel_ = mn;
        channelName_ = "mn";
    }
}

void LowPileupWSelector::SetComposite() {
    pfMetVec = TLorentzVector();
    pfMetVec.SetPtEtaPhiM(pfMet, 0., pfMetPhi, 0.);
    wCand = *lep + pfMetVec;
}

void LowPileupWSelector::FillHistograms(Long64_t entry, SystPair variation) { 
    if (lep->Pt() < 25)
        return;
    SafeHistFill(histMap1D_, getHistName("mW", variation.second), wCand.M(), weight);
    SafeHistFill(histMap1D_, getHistName("ptW", variation.second), wCand.Pt(), weight);
    SafeHistFill(histMap1D_, getHistName("yW", variation.second), wCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl", variation.second), lep->Pt(), weight);
    SafeHistFill(histMap1D_, getHistName("etal", variation.second), lep->Eta(), weight);
    SafeHistFill(histMap1D_, getHistName("pfMet", variation.second), pfMet, weight);

    //SafeHistFill(subprocessHistMaps1D_.at(name_+"_GenPtW_0_50"), getHistName("ptW", variation.second), wCand.Pt(), weight);
}
