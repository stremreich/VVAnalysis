#include "Analysis/VVAnalysis/interface/WGenSelector.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include <TStyle.h>
#include <regex>

void WGenSelector::Init(TTree *tree)
{
    allChannels_ = {"e", "m", "Unknown"};
    hists1D_ = {"CutFlow", "mW", "yW", "ptW", "ptl", "etal", "phil", "ptnu", "etanu", "phinu", "MET", "MET_phi",};
    weighthists1D_ = {"CutFlow", "mW", "yW", "ptW", "ptl", "etal", "phil", "ptnu", "etanu", "phinu", "MET", "MET_phi",};

    nLeptons_ = 1;
    NanoGenSelectorBase::Init(tree);
    
}

void WGenSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    NanoGenSelectorBase::LoadBranchesNanoAOD(entry, variation);

    if (leptons.size() > 0 && std::abs(leptons.at(0).pdgId()) == 11) {
        channel_ = e;
        channelName_ = "e";
    }
    else if (leptons.size() > 0 && std::abs(leptons.at(0).pdgId()) == 13) {
        channel_ = m;
        channelName_ = "m";
    }
    else {
        channel_ = Unknown;
        channelName_ = "Unknown";
        return;
    }
}

void WGenSelector::SetComposite() {
    if (leptons.size() == 0) {
        wCandMet = LorentzVector();
        wCand = LorentzVector();
        return;
    }
    else if (neutrinos.size() == 0) {
        wCand = LorentzVector();
        return;
    }
    wCandMet = leptons.at(0).polarP4() + genMet;
    wCand = neutrinos.size() > 0 ? leptons.at(0).polarP4() + neutrinos.at(0).polarP4() : LorentzVector();
}


void WGenSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    int step = 0;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (channel_ != m && channel_ != e) 
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (leptons.size() < 1)
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (neutrinos.size() < 1)
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    auto lep = leptons.at(0);
    if (std::abs(lep.eta() < 2.5))
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (lep.pt() < 25)
        return;

    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);
    if (genMet.pt() < 25)
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);


    SafeHistFill(histMap1D_, getHistName("mW", variation.second), wCand.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("yW", variation.second), wCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, getHistName("ptW", variation.second), wCand.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("MET", variation.second), genMet.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("MET_phi", variation.second), genMet.phi(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl", variation.second), lep.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("etal", variation.second), lep.eta(), weight);
    SafeHistFill(histMap1D_, getHistName("phil", variation.second), lep.phi(), weight);
    SafeHistFill(histMap1D_, getHistName("nJets", variation.second), jets.size(), weight);
    for (size_t i = 1; i <= 3; i++) {
        if (jets.size() >= i ) {
            const auto& jet = jets.at(i-1);
            SafeHistFill(histMap1D_, getHistName("ptj"+std::to_string(i), variation.second), jet.pt(), weight);
            SafeHistFill(histMap1D_, getHistName("etaj"+std::to_string(i), variation.second), jet.eta(), weight);
            SafeHistFill(histMap1D_, getHistName("phij"+std::to_string(i), variation.second), jet.phi(), weight);
        }  
    }

    if (variation.first == Central) {
        for (size_t i = 0; i < nLHEScaleWeight+nLHEPdfWeight; i++) {
            float thweight = i < nLHEScaleWeight ? LHEScaleWeight[i] : LHEPdfWeight[i-nLHEScaleWeight];
            thweight *= weight;
            SafeHistFill(weighthistMap1D_, getHistName("mW", variation.second), wCand.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("yW", variation.second), wCand.Rapidity(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("ptW", variation.second), wCand.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("MET", variation.second), genMet.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("MET_phi", variation.second), genMet.phi(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("ptl", variation.second), lep.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("etal", variation.second), lep.eta(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("phil", variation.second), lep.phi(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("nJets", variation.second), jets.size(), i, thweight);
        }
    }
}
