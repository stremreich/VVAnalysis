#include "Analysis/VVAnalysis/interface/ZGenSelector.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include <TStyle.h>
#include <regex>

void ZGenSelector::Init(TTree *tree)
{
    allChannels_ = {"ee", "mm"};
    // Add CutFlow for Unknown to understand when channels aren't categorized
    histMap1D_["CutFlow_Unknown"] = {};
    std::vector<std::string> basehists1D = {"CutFlow", "ZMass", "yZ", "ptZ", "ptl1", "etal1", "phil1", "ptl2", "etal2", "phil2", 
        "ptj1", "ptj2", "ptj3", "etaj1", "etaj2", "etaj3", "phij1", "phij2", "phij3", "nJets",
        "MET", "HT",};
    hists1D_ = basehists1D;
    std::vector<std::string> partonicChans = {"uu_dd", "uubar_ddbar", "ug_dg", "ubarg_dbarg", "gg", "other"};
    for (auto& chan : partonicChans) {
        for (auto& hist : basehists1D)
            hists1D_.push_back(chan + "_" + hist);
    }

    weighthists1D_ = {"ZMass", "yZ", "ptZ", "ptl1", "etal1", "phil1", "ptl2", "etal2", "phil2", 
        "ptj1", "ptj2", "ptj3", "etaj1", "etaj2", "etaj3", "phij1", "phij2", "phij3", "nJets",
        "MET", "HT", };
    nLeptons_ = 2;

    NanoGenSelectorBase::Init(tree);
    
}

void ZGenSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    NanoGenSelectorBase::LoadBranchesNanoAOD(entry, variation);
    b.SetBranch("Generator_id1", Generator_id1);
    b.SetBranch("Generator_id2", Generator_id2);

    if (leptons.size() < 2) {
        channel_ = Unknown;
        channelName_ = "Unknown";
        return;
    }
    if (leptons.at(0).pdgId() + leptons.at(1).pdgId() == 0) {
        if (std::abs(leptons.at(0).pdgId()) == 11) {
            channel_ = ee;
            channelName_ = "ee";
        }
        else if (std::abs(leptons.at(0).pdgId()) == 13) {
            channel_ = mm;
            channelName_ = "mm";
        }
    }
    else {
        channel_ = Unknown;
        channelName_ = "Unknown";
    }
}

void ZGenSelector::SetComposite() {
    if (leptons.size() >= 2)
        zCand = leptons.at(0).polarP4() + leptons.at(1).polarP4();
}

void ZGenSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    int step = 0;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (channel_ != mm && channel_ != ee) 
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    auto lep1 = leptons.at(0);
    auto lep2 = leptons.at(1);
    if (std::abs(lep1.eta()) > 2.5 || std::abs(lep2.eta()) > 2.5)
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (lep1.pt() < 25. || lep2.pt() < 25.)
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (zCand.mass() < 60. || zCand.mass() > 120.)
        return;

    std::string partonicChan = "other";
    if ((Generator_id1 == 1 && Generator_id2 == 1) || (Generator_id1 == 2 && Generator_id2 == 2))
        partonicChan = "uu_dd";
    else if ((Generator_id1 == 1 && Generator_id2 == -1) || (Generator_id1 == 2 && Generator_id2 == -2))
        partonicChan = "uubar_ddbar";
    else if (Generator_id1 == 21 && Generator_id2 == 21)
        partonicChan = "gg";
    else if ((Generator_id1 == 1 && Generator_id2 == 21) || (Generator_id1 == 21 && Generator_id2 == 1) || 
                (Generator_id1 == 2 && Generator_id2 == 21) || (Generator_id1 == 21 && Generator_id2 == 2))
        partonicChan = "ug_dg";
    else if ((Generator_id1 == -1 && Generator_id2 == 21) || (Generator_id1 == 21 && Generator_id2 == -1) || 
                (Generator_id1 == -2 && Generator_id2 == 21) || (Generator_id1 == 21 && Generator_id2 == -2))
        partonicChan = "ubarg_dbarg";

    SafeHistFill(histMap1D_, getHistName(partonicChan+"_ZMass", variation.second), zCand.mass(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_yZ", variation.second), zCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_ptZ", variation.second), zCand.pt(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_ptl1", variation.second), lep1.pt(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_etal1", variation.second), lep1.eta(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_phil1", variation.second), lep1.phi(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_ptl2", variation.second), lep2.pt(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_etal2", variation.second), lep2.eta(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_phil2", variation.second), lep2.phi(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_nJets", variation.second), jets.size(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_MET", variation.second), genMet.pt(), weight);
    SafeHistFill(histMap1D_, getHistName(partonicChan+"_HT", variation.second), ht, weight);
    for (size_t i = 1; i <= 3; i++) {
        if (jets.size() >= i ) {
            const auto& jet = jets.at(i-1);
            SafeHistFill(histMap1D_, getHistName(partonicChan+"_ptj"+std::to_string(i), variation.second), jet.pt(), weight);
            SafeHistFill(histMap1D_, getHistName(partonicChan+"_etaj"+std::to_string(i), variation.second), jet.eta(), weight);
            SafeHistFill(histMap1D_, getHistName(partonicChan+"_phij"+std::to_string(i), variation.second), jet.phi(), weight);
        }  
    }

    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);
    SafeHistFill(histMap1D_, getHistName("ZMass", variation.second), zCand.mass(), weight);
    SafeHistFill(histMap1D_, getHistName("yZ", variation.second), zCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, getHistName("ptZ", variation.second), zCand.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl1", variation.second), lep1.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("etal1", variation.second), lep1.eta(), weight);
    SafeHistFill(histMap1D_, getHistName("phil1", variation.second), lep1.phi(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl2", variation.second), lep2.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("etal2", variation.second), lep2.eta(), weight);
    SafeHistFill(histMap1D_, getHistName("phil2", variation.second), lep2.phi(), weight);
    SafeHistFill(histMap1D_, getHistName("nJets", variation.second), jets.size(), weight);
    SafeHistFill(histMap1D_, getHistName("MET", variation.second), genMet.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("HT", variation.second), ht, weight);
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
            //thweight *= weight/(LHEWeight > 0 ? LHEWeight : 1);
            SafeHistFill(weighthistMap1D_, getHistName("ZMass", variation.second), zCand.mass(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("yZ", variation.second), zCand.Rapidity(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("ptZ", variation.second), zCand.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("ptl1", variation.second), lep1.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("etal1", variation.second), lep1.eta(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("phil1", variation.second), lep1.phi(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("ptl2", variation.second), lep2.pt(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("etal2", variation.second), lep2.eta(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("phil2", variation.second), lep2.phi(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("nJets", variation.second), jets.size(), i, thweight);
            SafeHistFill(weighthistMap1D_, getHistName("MET", variation.second), genMet.pt(), i, thweight);
        }
    }
}

