#include "Analysis/VVAnalysis/interface/ZSelector.h"
#include "TLorentzVector.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

void ZSelector::Init(TTree *tree)
{
    allChannels_ = {"ee", "mm", };
    hists1D_ = {"ZMass", "ptl1", "etal1", "ptl2", "etal2"};

    SelectorBase::Init(tree);
    
    singleLepton_ = false;
    if (!isMC_ && name_.find("Single") != std::string::npos)
        singleLepton_ = true;
}

void ZSelector::SetBranchesUWVV() {
    if (isMC_){
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
        fChain->SetBranchAddress("nTruePU", &nTruePU, &b_nTruePU);
    }
    fChain->SetBranchAddress("Mass", &ZMass, &b_ZMass);

    if (channel_ == ee) {
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == mm) {
        fChain->SetBranchAddress("m1IsWZTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m2IsWZTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
    }

    fChain->SetBranchAddress("type1_pfMETEt", &MET, &b_MET);
    fChain->SetBranchAddress("type1_pfMETPhi", &type1_pfMETPhi, &b_type1_pfMETPhi);
    //fChain->SetBranchAddress("nCBVIDTightElec", &nCBVIDTightElec, &b_nCBVIDTightElec);
    //fChain->SetBranchAddress("nCBVIDHLTSafeElec", &nCBVIDHLTSafeElec, &b_nCBVIDHLTSafeElec);
    //fChain->SetBranchAddress("nWZTightMuon", &nWZTightMuon, &b_nWZTightMuon);
    //fChain->SetBranchAddress("nWZMediumMuon", &nWZMediumMuon, &b_nWZMediumMuon);
}

void ZSelector::SetBranchesNanoAOD() {
    fChain->SetBranchAddress("nElectron", &nElectron, &b_nElectron);
    fChain->SetBranchAddress("nMuon", &nMuon, &b_nMuon);
    fChain->SetBranchAddress("Electron_pt", &Electron_pt, &b_Electron_pt);
    fChain->SetBranchAddress("Electron_eta", &Electron_eta, &b_Electron_eta);
    fChain->SetBranchAddress("Electron_phi", &Electron_phi, &b_Electron_phi);
    fChain->SetBranchAddress("Muon_pt", &Muon_pt, &b_Muon_pt);
    fChain->SetBranchAddress("Muon_eta", &Muon_eta, &b_Muon_eta);
    fChain->SetBranchAddress("Muon_phi", &Muon_phi, &b_Muon_phi);
    fChain->SetBranchAddress("Electron_cutBased", &Electron_cutBased, &b_Electron_cutBased);
    fChain->SetBranchAddress("Muon_tightId", &Muon_tightId, &b_Muon_tightId);
    fChain->SetBranchAddress("Muon_mediumId", &Muon_mediumId, &b_Muon_mediumId);
    fChain->SetBranchAddress("Muon_pfRelIso04_all", &Muon_pfRelIso04_all, &b_Muon_pfRelIso04_all);
    fChain->SetBranchAddress("MET_pt", &MET, &b_MET);
    fChain->SetBranchAddress("MET_phi", &type1_pfMETPhi, &b_type1_pfMETPhi);
    fChain->SetBranchAddress("Electron_charge", &Electron_charge, &b_Electron_charge);
    fChain->SetBranchAddress("Muon_charge", &Muon_charge, &b_Muon_charge);
    fChain->SetBranchAddress("Electron_mass", &Electron_mass, &b_Electron_mass);
    fChain->SetBranchAddress("Muon_mass", &Muon_mass, &b_Muon_mass);
    fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL", &Dimuon_Trigger, &b_Dimuon_Trigger);
    fChain->SetBranchAddress("HLT_Mu27", &SingleMuon_Trigger, &b_SingleMuon_Trigger);
    //fChain->SetBranchAddress("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", &Dielectron_Trigger, &b_Dielectron_Trigger);
    //fChain->SetBranchAddress("HLT_Ele27_WPLoose_Gsf", &SingleElectron_Trigger, &b_SingleElectron_Trigger);
    if (isMC_) {
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
        fChain->SetBranchAddress("Pileup_nPU", &numPU, &b_numPU);
    }
}

void ZSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    b_nElectron->GetEntry(entry);
    b_nMuon->GetEntry(entry);
    b_Electron_pt->GetEntry(entry);
    b_Electron_eta->GetEntry(entry);
    b_Electron_phi->GetEntry(entry);
    b_Muon_pt->GetEntry(entry);
    b_Muon_eta->GetEntry(entry);
    b_Muon_phi->GetEntry(entry);
    b_Electron_cutBased->GetEntry(entry);
    b_Muon_tightId->GetEntry(entry);
    b_Muon_mediumId->GetEntry(entry);
    b_Muon_pfRelIso04_all->GetEntry(entry);
    b_Electron_charge->GetEntry(entry);
    b_Muon_charge->GetEntry(entry);
    b_Electron_mass->GetEntry(entry);
    b_Muon_mass->GetEntry(entry);
    b_MET->GetEntry(entry);
    b_Dimuon_Trigger->GetEntry(entry);
    //b_SingleMuon_Trigger->GetEntry(entry);
    //b_SingleElectron_Trigger->GetEntry(entry);
    //b_Dielectron_Trigger->GetEntry(entry);

    if (nElectron > N_KEEP_MU_E_ || nMuon > N_KEEP_MU_E_) {
        std::string message = "Found more electrons or muons than max read number.\n    Found ";
        message += std::to_string(nElectron);
        message += " electrons.\n    Found ";
        message += std::to_string(nMuon);
        message += " Muons\n  --> Max read number was ";
        message += std::to_string(N_KEEP_MU_E_);
        message += "\nExiting because this can cause problems. Increase N_KEEP_MU_E_ to avoid this error.\n";
        throw std::domain_error(message);
    }

    ZMass = 0;
    l1Pt = 0;
    l2Pt = 0;
    l1Eta = 0;
    l2Eta = 0;
    l1Phi = 0;
    l2Phi = 0;
    l1Mass = 0;
    l2Mass = 0;

    // cut-based ID Fall17 V2 (0:fail, 1:veto, 2:loose, 3:medium, 4:tight)
    nCBVIDTightElec = std::count(Electron_cutBased, Electron_cutBased+nElectron, 4);
    nCBVIDVetoElec = std::count(Electron_cutBased, Electron_cutBased+nElectron, 1);
    nTightIdMuon = std::count(Muon_tightId, Muon_tightId+nMuon, true);
    nMediumIdMuon = std::count(Muon_mediumId, Muon_mediumId+nMuon, true);
    //nTightIsoMuon = std::count(Muon_pfIsoId, Muon_pfIsoId+nMuon, 4);
    //nLooseIsoMuon = std::count(Muon_pfIsoId, Muon_pfIsoId+nMuon, 1);

    channel_ = channelMap_[channelName_];
    std::vector<size_t> goodIndices = {};

    if (nTightIdMuon >= 2) {
        channel_ = mm;
        channelName_ = "mm";
        if (!(Muon_tightId[0] && Muon_tightId[1])) {
            for (size_t i = 0; i < nMuon; i++) {
                if (Muon_tightId[i])
                    goodIndices.push_back(i);
            }
            if (goodIndices.size() < 2) {
                return;
            }
        }
        else 
            goodIndices = {0, 1};
        if (Muon_charge[goodIndices[0]] != Muon_charge[goodIndices[1]]) {
            l1Pt = Muon_pt[goodIndices[0]];
            l2Pt = Muon_pt[goodIndices[1]];
            l1Eta = Muon_eta[goodIndices[0]];
            l2Eta = Muon_eta[goodIndices[1]];
            l1Phi = Muon_phi[goodIndices[0]];
            l2Phi = Muon_phi[goodIndices[1]];
            l1Mass = Muon_mass[goodIndices[0]];
            l2Mass = Muon_mass[goodIndices[1]];
            l1IsTight = (Muon_tightId[goodIndices[0]] && (Muon_pfRelIso04_all[goodIndices[0]] < 0.15));
            l2IsTight = (Muon_tightId[goodIndices[1]] && (Muon_pfRelIso04_all[goodIndices[1]] < 0.15));
        }
    }
    else if (nCBVIDTightElec >= 2) {
        channel_ = ee;
        channelName_ = "ee";
        if (!(Electron_cutBased[0] == 4 && Electron_cutBased[1] == 4)) {
            for (size_t i = 0; i < nElectron; i++) {
                if (Electron_cutBased[i] == 4)
                    goodIndices.push_back(i);
            }
            if (goodIndices.size() < 2)
                return;
        }
        else 
            goodIndices = {0, 1};
        if (Electron_charge[goodIndices[0]] != Electron_charge[goodIndices[1]]) {
            l1Pt = Electron_pt[goodIndices[0]];
            l2Pt = Electron_pt[goodIndices[1]];
            l1Eta = Electron_eta[goodIndices[0]];
            l2Eta = Electron_eta[goodIndices[1]];
            l1Phi = Electron_phi[goodIndices[0]];
            l2Phi = Electron_phi[goodIndices[1]];
            l1Mass = Electron_mass[goodIndices[0]];
            l2Mass = Electron_mass[goodIndices[1]];
            l1IsTight = (Electron_cutBased[goodIndices[0]] == 4);
            l2IsTight = (Electron_cutBased[goodIndices[1]] == 4);
        }
    }
    SetMass();

    if (isMC_) {
        b_genWeight->GetEntry(entry);
        //TODO: add scale factors
        //b_numPU->GetEntry(entry);
        //b_l1GenPt->GetEntry(entry);
        //b_l2GenPt->GetEntry(entry);
        //b_l3GenPt->GetEntry(entry);
        ApplyScaleFactors();
    }
    else {
        //TODO: add MET filters
        //b_Flag_duplicateMuonsPass->GetEntry(entry);          
        //b_Flag_badMuonsPass->GetEntry(entry);          
    }


    //    passesTrigger = isMC_ ? Dimuon_Trigger : true;;
    if (!singleLepton_)
       passesTrigger = (Dimuon_Trigger || SingleMuon_Trigger ||
              Dielectron_Trigger || SingleElectron_Trigger);
    //       passesTrigger = (Dimuon_Trigger || SingleMuon_Trigger);
    else
      // passesTrigger = (!Dimuon_Trigger && SingleMuon_Trigger);
       passesTrigger = ((!Dimuon_Trigger && SingleMuon_Trigger) ||
              (!Dielectron_Trigger && SingleElectron_Trigger));

    passesLeptonVeto = (std::min(nMediumIdMuon, nLooseIsoMuon) + nCBVIDVetoElec) == 2;
}

void ZSelector::LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation){ 
    weight = 1;
    b_l1Pt->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l2Eta->GetEntry(entry);

    if (isMC_) {
        b_genWeight->GetEntry(entry);
    }
    b_ZMass->GetEntry(entry);
    b_l1IsTight->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_MET->GetEntry(entry);
    //b_nCBVIDTightElec->GetEntry(entry);
    //
    passesTrigger = true;
}

void ZSelector::ApplyScaleFactors() {
    if (isMC_)
        weight = genWeight;
    weight = 1;
    return;
    // This will come later
    if (channel_ == ee) {
        weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    }
    else if (channel_ == mm) {
        weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    }
    weight *= pileupSF_->Evaluate1D(numPU);
}

void ZSelector::SetMass() {
    if (l1Pt == 0. || l2Pt == 0.) {
        return;
    }
    TLorentzVector lepton1;
    lepton1.SetPtEtaPhiM(l1Pt, l1Eta, l1Phi, l1Mass);
    TLorentzVector lepton2;
    lepton2.SetPtEtaPhiM(l2Pt, l2Eta, l2Phi, l2Mass);
    ZMass = (lepton1+lepton2).M();
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool ZSelector::zlep1IsTight() {
    return l1IsTight; 
}

bool ZSelector::zlep2IsTight() {
    return l2IsTight; 
}

bool ZSelector::tightZLeptons() {
    return zlep1IsTight() && zlep2IsTight(); 
}

void ZSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    //cutflow_ee_->Fill(0.,weight);
    //SafeHistFill(histMap1D_, getHistName("CutFlow_ee", variation.second), 0., weight);
    //cutflow_mm_->Fill(0.,weight);
    //if (channel_ == ee)
    //    cutflow_ee_->Fill(1.,weight);
    //else if (channel_ == mm)
    //    cutflow_mm_->Fill(1.,weight);
  SafeHistFill(histMap1D_, getHistName("ptl2", variation.second), l2Pt, weight);
    if (!passesTrigger)
        return;
    //for (auto& hist : histMap1D_)
    if (channel_ != mm && channel_ != ee) 
        return;

    if (channel_ == ee && (std::abs(l1Eta) > 2.4 || std::abs(l2Eta) > 2.4 ))
        return;
    else if (channel_ == mm && (std::abs(l1Eta) > 2.5 || std::abs(l2Eta) > 2.5 ))
        return;
    //if (channel_ == ee)
    //    cutflow_ee_->Fill(2,weight);
    //else if (channel_ == mm)
    //    cutflow_mm_->Fill(2,weight);

    if (l1Pt < 25 || l2Pt < 15)
        return;
    //if (channel_ == ee)
    //    cutflow_ee_->Fill(3,weight);
    //else if (channel_ == mm)
    //    cutflow_mm_->Fill(3,weight);

    if (ZMass > 101.1876 || ZMass < 81.1876)
        return;

    //if (channel_ == ee)
    //    cutflow_ee_->Fill(4,weight);
    //else if (channel_ == mm)
    //    cutflow_mm_->Fill(4,weight);

    if (MET > 25)
        return;

    //if (channel_ == ee)
    //    cutflow_ee_->Fill(5,weight);
    //else if (channel_ == mm)
    //    cutflow_mm_->Fill(5,weight);

    if (!tightZLeptons())
        return;

    //if (channel_ == ee)
    //    cutflow_ee_->Fill(6,weight);
    //else if (channel_ == mm)
    //    cutflow_mm_->Fill(6,weight);

    SafeHistFill(histMap1D_, getHistName("ZMass", variation.second), ZMass, weight);
    SafeHistFill(histMap1D_, getHistName("ptl1", variation.second), l1Pt, weight);

}

void ZSelector::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}
