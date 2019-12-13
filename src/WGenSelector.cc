#include "Analysis/VVAnalysis/interface/WGenSelector.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include <TStyle.h>
#include <regex>
#include <cmath>

void WGenSelector::Init(TTree *tree)
{
    histMap1D_[{"CutFlow", Unknown, Central}] = {};
    allChannels_ = {{ep, "ep"}, {en, "en"}, {mp, "mp"}, {mn, "mn"}};
    hists1D_ = {"CutFlow", "mWmet", "yWmet", "ptWmet", "mW", "yW", "ptW", "mTtrue", "mTmet",
        "ptl", "etal", "phil", "ptnu", "etanu", "phinu", "MET", "MET_phi",
        "ptj1", "ptj2", "etaj1", "etaj2", "nJets"};
    weighthists1D_ = {"mWmet", "yWmet", "ptWmet", "mW", "yW", "ptW", "mTtrue", "mTmet",
        "ptl", "etal", "phil", "ptnu", "etanu", "phinu", "MET", "MET_phi", "nJets"};

    nLeptons_ = 1;
    NanoGenSelectorBase::Init(tree);
    
}

void WGenSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    NanoGenSelectorBase::LoadBranchesNanoAOD(entry, variation);

    if (leptons.size() > 0 && std::abs(leptons.at(0).pdgId()) == 11) {
        if (leptons.at(0).pdgId() < 0) {
            channel_ = en;
            channelName_ = "en";
        }
        else {
            channel_ = ep;
            channelName_ = "ep";
        }
    }
    else if (leptons.size() > 0 && std::abs(leptons.at(0).pdgId()) == 13) {
        if (leptons.at(0).pdgId() < 0) {
            channel_ = mn;
            channelName_ = "mn";
        }
        else {
            channel_ = mp;
            channelName_ = "mp";
        }
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
    auto lepP4 = leptons.at(0).polarP4();
    auto nuP4 = neutrinos.size() > 0 ? neutrinos.at(0).polarP4() : LorentzVector();
    wCandMet = lepP4 + genMet;
    wCand = neutrinos.size() > 0 ? lepP4 + nuP4 : LorentzVector();
    auto mt = [] (LorentzVector& l, LorentzVector& v) {
        return std::sqrt(2*l.pt()*v.pt()*(1 - cos(l.phi() - v.phi())));
    };
    mTtrue = mt(lepP4, nuP4);
    mTmet = mt(lepP4, genMet);
}


void WGenSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    int step = 0;
    SafeHistFill(histMap1D_, "CutFlow", channel_, variation.first, step++, weight);

    if (channel_ != mn && channel_ != en && channel_ != mp && channel_ != ep) 
        return;
    SafeHistFill(histMap1D_, "CutFlow", channel_, variation.first, step++, weight);

    if (leptons.size() < 1)
        return;
    SafeHistFill(histMap1D_, "CutFlow", channel_, variation.first, step++, weight);

    if (neutrinos.size() < 1)
        return;
    SafeHistFill(histMap1D_, "CutFlow", channel_, variation.first, step++, weight);

    auto lep = leptons.at(0);
    if (std::abs(lep.eta() > 2.5))
        return;
    SafeHistFill(histMap1D_, "CutFlow", channel_, variation.first, step++, weight);

    if (lep.pt() < 25)
        return;

    SafeHistFill(histMap1D_, "mW", channel_, variation.first, wCand.mass(), weight);
    SafeHistFill(histMap1D_, "yW", channel_, variation.first, wCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, "ptW", channel_, variation.first, wCand.pt(), weight);
    SafeHistFill(histMap1D_, "mTtrue", channel_, variation.first, mTtrue, weight);
    SafeHistFill(histMap1D_, "mTmet", channel_, variation.first, mTmet, weight);
    SafeHistFill(histMap1D_, "mWmet", channel_, variation.first, wCandMet.mass(), weight);
    SafeHistFill(histMap1D_, "yWmet", channel_, variation.first, wCandMet.Rapidity(), weight);
    SafeHistFill(histMap1D_, "ptWmet", channel_, variation.first, wCandMet.pt(), weight);
    SafeHistFill(histMap1D_, "MET", channel_, variation.first, genMet.pt(), weight);
    SafeHistFill(histMap1D_, "MET_phi", channel_, variation.first, genMet.phi(), weight);
    SafeHistFill(histMap1D_, "ptl", channel_, variation.first, lep.pt(), weight);
    SafeHistFill(histMap1D_, "etal", channel_, variation.first, lep.eta(), weight);
    SafeHistFill(histMap1D_, "phil", channel_, variation.first, lep.phi(), weight);
    SafeHistFill(histMap1D_, "ptnu", channel_, variation.first, lep.pt(), weight);
    SafeHistFill(histMap1D_, "etanu", channel_, variation.first, lep.eta(), weight);
    SafeHistFill(histMap1D_, "phinu", channel_, variation.first, lep.phi(), weight);
    SafeHistFill(histMap1D_, "nJets", channel_, variation.first, jets.size(), weight);
    for (size_t i = 1; i <= 3; i++) {
        if (jets.size() >= i ) {
            const auto& jet = jets.at(i-1);
            SafeHistFill(histMap1D_, ("ptj"+std::to_string(i)).c_str(), channel_, variation.first, jet.pt(), weight);
            SafeHistFill(histMap1D_, ("etaj"+std::to_string(i)).c_str(), channel_, variation.first, jet.eta(), weight);
            SafeHistFill(histMap1D_, ("phij"+std::to_string(i)).c_str(), channel_, variation.first, jet.phi(), weight);
        }  
    }

    if (variation.first == Central) {
        size_t maxEntry = *nLHEScaleWeight+*nLHEPdfWeight;
        if (doMC2H_ == true)
            maxEntry += N_MC2HESSIAN_WEIGHTS_;

        for (size_t i = 0; i < maxEntry; i++) {
            float thweight = (i < *nLHEScaleWeight) ? LHEScaleWeight[i] : ( i < (*nLHEPdfWeight+*nLHEScaleWeight) ? LHEPdfWeight[i-*nLHEScaleWeight] 
                    : LHEHessianPdfWeight[i-*nLHEScaleWeight-*nLHEPdfWeight]);
            if (i > *nLHEScaleWeight && i < *nLHEScaleWeight+*nLHEPdfWeight)
                mcPdfWeights_->Fill(thweight);
            else if (i > *nLHEScaleWeight+*nLHEPdfWeight)
                hesPdfWeights_->Fill(thweight);
            else 
                scaleWeights_->Fill(thweight);
            thweight *= weight;
            SafeHistFill(weighthistMap1D_, "mW", channel_, variation.first, wCand.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, "yW", channel_, variation.first, wCand.Rapidity(), i, thweight);
            SafeHistFill(weighthistMap1D_, "ptW", channel_, variation.first, wCand.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, "mWmet", channel_, variation.first, wCandMet.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, "yWmet", channel_, variation.first, wCandMet.Rapidity(), i, thweight);
            SafeHistFill(weighthistMap1D_, "ptWmet", channel_, variation.first, wCandMet.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, "MET", channel_, variation.first, genMet.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, "MET_phi", channel_, variation.first, genMet.phi(), i, thweight);
            SafeHistFill(weighthistMap1D_, "ptl", channel_, variation.first, lep.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, "etal", channel_, variation.first, lep.eta(), i, thweight);
            SafeHistFill(weighthistMap1D_, "phil", channel_, variation.first, lep.phi(), i, thweight);
            SafeHistFill(weighthistMap1D_, "nJets", channel_, variation.first, jets.size(), i, thweight);
        }
    }
}
