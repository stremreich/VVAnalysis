#include "Analysis/VVAnalysis/interface/WZSelectorBase.h"
#include "TLorentzVector.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

// This is very WZ specific and should really be improved or likely removed
std::string WZSelectorBase::GetNameFromFile() {
    std::regex expr = std::regex("201[0-9]-[0-9][0-9]-[0-9][0-9]-(.*)-WZxsec2016");
    std::smatch matches;
    std::string fileName = fChain->GetTree()->GetDirectory()->GetFile()->GetName(); 

    std::regex_search(fileName, matches, expr);
    return std::string(matches.str(1));
}

void WZSelectorBase::SetScaleFactors() {
    pileupSF_ = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
    if (pileupSF_ == nullptr ) 
        std::invalid_argument("Must pass pileup weights SF");
    eIdSF_ = (ScaleFactor *) GetInputList()->FindObject("electronTightIdSF");
    if (eIdSF_ == nullptr ) 
        std::invalid_argument("Must pass electron ID SF");
    eGsfSF_ = (ScaleFactor *) GetInputList()->FindObject("electronGsfSF");
    if (eGsfSF_ == nullptr ) 
        std::invalid_argument("Must pass electron GSF SF");
    mIdSF_ = (ScaleFactor *) GetInputList()->FindObject("muonTightIdSF");
    if (mIdSF_ == nullptr ) 
        std::invalid_argument("Must pass muon ID SF");
    mIsoSF_ = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
    if (mIsoSF_ == nullptr ) 
        std::invalid_argument("Must pass muon Iso SF");

    prefireEff_ = (TEfficiency*) GetInputList()->FindObject("prefireEfficiencyMap");
    if (prefireEff_ == nullptr ) 
        std::invalid_argument("Must pass prefiring efficiency map");
}

void WZSelectorBase::Init(TTree *tree)
{
    isVBS_ = (selection_ == VBSselection_Loose || 
        selection_ == VBSselection_Loose_Full || 
        selection_ == VBSselection_NoZeppenfeld || 
        selection_ == VBSselection_NoZeppenfeld_Full || 
        selection_ == VBSselection_Tight || 
        selection_ == VBSselection_Tight_Full || 
        selection_ == VBSBackgroundControl || 
        selection_ == VBSBackgroundControl_Full || 
        selection_ == VBSBackgroundControlATLAS || 
        selection_ == VBSBackgroundControlLoose ||
        selection_ == VBSBackgroundControlLoose_Full
        );

    allChannels_ = {"eee", "eem", "emm", "mmm"};

    if (isMC_){
        isNonpromptMC_ = false;
        isZgamma_ = false;
        if (std::find(nonprompt3l_.begin(), nonprompt3l_.end(), name_) != nonprompt3l_.end()) {
            isNonpromptMC_ = true;
        }
        else if (name_ == "zg") {
            isZgamma_ = true;
        }
    }
    SelectorBase::Init(tree);
}

void WZSelectorBase::SetBranchesUWVV() {
    if (isMC_){
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
        fChain->SetBranchAddress("nTruePU", &nTruePU, &b_nTruePU);
    }
    else {
        fChain->SetBranchAddress("Flag_duplicateMuonsPass", &Flag_duplicateMuonsPass, &b_Flag_duplicateMuonsPass);
        fChain->SetBranchAddress("Flag_badMuonsPass", &Flag_badMuonsPass, &b_Flag_badMuonsPass);
    }

    if (channel_ == eee) {
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e3IsCBVIDTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e3MtToMET", &l3MtToMET, &b_l3MtToMET);
        if (isMC_) {
            fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
            fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
            fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        }
    }
    else if (channel_ == eem) {
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("mIsWZTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("mMtToMET", &l3MtToMET, &b_l3MtToMET);
        if (isMC_) {
            fChain->SetBranchAddress("mGenPt", &l3GenPt, &b_l3GenPt);
            fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
            fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        }
    }
    else if (channel_ == emm) {
        fChain->SetBranchAddress("eIsCBVIDTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1IsWZTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m2IsWZTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("eMtToMET", &l3MtToMET, &b_l3MtToMET);
        if (isMC_) {
            fChain->SetBranchAddress("eGenPt", &l3GenPt, &b_l3GenPt);
            fChain->SetBranchAddress("m1GenPt", &l1GenPt, &b_l1GenPt);
            fChain->SetBranchAddress("m2GenPt", &l2GenPt, &b_l2GenPt);
        }
    }
    else if (channel_ == mmm) {
        fChain->SetBranchAddress("m1IsWZTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m2IsWZTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m3IsWZTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m3MtToMET", &l3MtToMET, &b_l3MtToMET);
        if (isMC_) {
            fChain->SetBranchAddress("m1GenPt", &l1GenPt, &b_l1GenPt);
            fChain->SetBranchAddress("m2GenPt", &l2GenPt, &b_l2GenPt);
            fChain->SetBranchAddress("m3GenPt", &l3GenPt, &b_l3GenPt);
        }
    }

    fChain->SetBranchAddress("type1_pfMETEt", &MET, &b_MET);
    fChain->SetBranchAddress("type1_pfMETPhi", &type1_pfMETPhi, &b_type1_pfMETPhi);
    fChain->SetBranchAddress("nCBVIDTightElec", &nCBVIDTightElec, &b_nCBVIDTightElec);
    fChain->SetBranchAddress("nCBVIDHLTSafeElec", &nCBVIDHLTSafeElec, &b_nCBVIDHLTSafeElec);
    fChain->SetBranchAddress("nCBVIDVetoElec", &nCBVIDVetoElec, &b_nCBVIDVetoElec);
    fChain->SetBranchAddress("nWZTightMuon", &nWZTightMuon, &b_nWZTightMuon);
    fChain->SetBranchAddress("nWZMediumMuon", &nWZMediumMuon, &b_nWZMediumMuon);
    fChain->SetBranchAddress("Flag_BadChargedCandidateFilterPass", &Flag_BadChargedCandidateFilterPass, &b_Flag_BadChargedCandidateFilterPass);
    fChain->SetBranchAddress("Flag_BadPFMuonFilterPass", &Flag_BadPFMuonFilterPass, &b_Flag_BadPFMuonFilterPass);
    fChain->SetBranchAddress("Flag_HBHENoiseFilterPass", &Flag_HBHENoiseFilterPass, &b_Flag_HBHENoiseFilterPass);
    fChain->SetBranchAddress("Flag_HBHENoiseIsoFilterPass", &Flag_HBHENoiseIsoFilterPass, &b_Flag_HBHENoiseIsoFilterPass);
    fChain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilterPass", &Flag_EcalDeadCellTriggerPrimitiveFilterPass, &b_Flag_EcalDeadCellTriggerPrimitiveFilterPass);
    fChain->SetBranchAddress("Flag_goodVerticesPass", &Flag_goodVerticesPass, &b_Flag_goodVerticesPass);
    fChain->SetBranchAddress("Flag_eeBadScFilterPass", &Flag_eeBadScFilterPass, &b_Flag_eeBadScFilterPass);
    fChain->SetBranchAddress("Flag_globalTightHalo2016FilterPass", &Flag_globalTightHalo2016FilterPass, &b_Flag_globalTightHalo2016FilterPass);
}

void WZSelectorBase::SetBranchesNanoAOD() {
    fChain->SetBranchAddress("nElectron", &nElectron, &b_nElectron);
    fChain->SetBranchAddress("nMuon", &nMuon, &b_nMuon);
    fChain->SetBranchAddress("Electron_pt", &Electron_pt, &b_Electron_pt);
    fChain->SetBranchAddress("Electron_eta", &Electron_eta, &b_Electron_eta);
    fChain->SetBranchAddress("Electron_phi", &Electron_phi, &b_Electron_phi);
    fChain->SetBranchAddress("Electron_mass", &Electron_mass, &b_Electron_mass);
    fChain->SetBranchAddress("Muon_pt", &Muon_pt, &b_Muon_pt);
    fChain->SetBranchAddress("Muon_eta", &Muon_eta, &b_Muon_eta);
    fChain->SetBranchAddress("Muon_phi", &Muon_phi, &b_Muon_phi);
    fChain->SetBranchAddress("Muon_mass", &Muon_mass, &b_Muon_mass);
    fChain->SetBranchAddress("Muon_pfRelIso04_all", &Muon_pfRelIso04_all, &b_Muon_pfRelIso04_all);
    fChain->SetBranchAddress("Electron_cutBased", &Electron_cutBased, &b_Electron_cutBased);
    fChain->SetBranchAddress("Muon_tightId", &Muon_tightId, &b_Muon_tightId);
    fChain->SetBranchAddress("Muon_tkIsoId", &Muon_tkIsoId, &b_Muon_tkIsoId);
    fChain->SetBranchAddress("MET_pt", &MET, &b_MET);
    fChain->SetBranchAddress("MET_phi", &type1_pfMETPhi, &b_type1_pfMETPhi);
    fChain->SetBranchAddress("Electron_charge", &Electron_charge, &b_Electron_charge);
    fChain->SetBranchAddress("Muon_charge", &Muon_charge, &b_Muon_charge);
    if (isMC_) {
        //fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
        fChain->SetBranchAddress("Pileup_nPU", &numPU, &b_numPU);
    }
    //else {
    //    fChain->SetBranchAddress("Flag_duplicateMuonsPass", &Flag_duplicateMuonsPass, &b_Flag_duplicateMuonsPass);
    //    fChain->SetBranchAddress("Flag_badMuonsPass", &Flag_badMuonsPass, &b_Flag_badMuonsPass);
    //}
    //fChain->SetBranchAddress("Flag_BadChargedCandidateFilterPass", &Flag_BadChargedCandidateFilterPass, &b_Flag_BadChargedCandidateFilterPass);
    //fChain->SetBranchAddress("Flag_BadPFMuonFilterPass", &Flag_BadPFMuonFilterPass, &b_Flag_BadPFMuonFilterPass);
    //fChain->SetBranchAddress("Flag_HBHENoiseFilterPass", &Flag_HBHENoiseFilterPass, &b_Flag_HBHENoiseFilterPass);
    //fChain->SetBranchAddress("Flag_HBHENoiseIsoFilterPass", &Flag_HBHENoiseIsoFilterPass, &b_Flag_HBHENoiseIsoFilterPass);
    //fChain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilterPass", &Flag_EcalDeadCellTriggerPrimitiveFilterPass, &b_Flag_EcalDeadCellTriggerPrimitiveFilterPass);
    //fChain->SetBranchAddress("Flag_goodVerticesPass", &Flag_goodVerticesPass, &b_Flag_goodVerticesPass);
    //fChain->SetBranchAddress("Flag_eeBadScFilterPass", &Flag_eeBadScFilterPass, &b_Flag_eeBadScFilterPass);
    //fChain->SetBranchAddress("Flag_globalTightHalo2016FilterPass", &Flag_globalTightHalo2016FilterPass, &b_Flag_globalTightHalo2016FilterPass);
}

void WZSelectorBase::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
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
    b_Muon_tightId->GetEntry(entry);
    b_Muon_pfRelIso04_all->GetEntry(entry);
    b_Electron_charge->GetEntry(entry);
    b_Muon_charge->GetEntry(entry);
    b_Electron_mass->GetEntry(entry);
    b_Muon_mass->GetEntry(entry);
    b_MET->GetEntry(entry);

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

    l1Pt = 0;
    l2Pt = 0;
    l3Pt = 0;
    l1Eta = 0;
    l2Eta = 0;
    l3Eta = 0;
    l1Phi = 0;
    l2Phi = 0;
    l3Phi = 0;
    l1Mass = 0;
    l2Mass = 0;
    l3Mass = 0;

    SetChannelAndIndicesNano();
    if (channel_ != eee && channel_ != eem && channel_ != emm && channel_ != mmm) {
        passesLeptonVeto = false;
        return;
    }
    else
        passesLeptonVeto = true;

    SetGoodLeptonsFromNano();
    SetLeptonVarsNano();
    SetMasses();

    if (isMC_) {
        b_genWeight->GetEntry(entry);
        b_numPU->GetEntry(entry);
        //ApplyScaleFactors();
    }
    //else {
    //}

}

void WZSelectorBase::SetLeptonVarsNano() {
    auto& l1 = leptons.at(0);
    auto& l2 = leptons.at(1);
    auto& l3 = leptons.at(2);

    l1Pt = l1.pt();
    l1Eta = l1.eta();
    l1Phi = l1.phi();
    l1Mass= l1.mass();
    l2Pt = l2.pt();
    l2Eta = l2.eta();
    l2Phi = l2.phi();
    l2Mass= l2.mass();
    l3Pt = l3.pt();
    l3Eta = l3.eta();
    l3Phi = l3.phi();
    l3Mass= l3.mass();

    if (channel_ == eee) {
        l1IsTight = (Electron_cutBased[looseElecIndices.at(0)] == 4);
        l2IsTight = (Electron_cutBased[looseElecIndices.at(1)] == 4);
        l3IsTight = (Electron_cutBased[looseElecIndices.at(2)] == 4);
    }
    else if (channel_ == eem) {
        l1IsTight = (Electron_cutBased[looseElecIndices.at(0)] == 4);
        l2IsTight = (Electron_cutBased[looseElecIndices.at(1)] == 4);
        size_t imu = looseMuonIndices.at(0);
        l3IsTight = (Muon_tightId[imu] && Muon_pfRelIso04_all[imu] < 0.15);
    }
    else if (channel_ == eem) {
        size_t imu = looseMuonIndices.at(0);
        l1IsTight = (Muon_tightId[imu] && Muon_pfRelIso04_all[imu] < 0.15);
        imu = looseMuonIndices.at(1);
        l2IsTight = (Muon_tightId[imu] && Muon_pfRelIso04_all[imu] < 0.15);
        l3IsTight = (Electron_cutBased[looseElecIndices.at(1)] == 4);
     }
     else if (channel_ == mmm) {
        size_t imu = looseMuonIndices.at(0);
        l1IsTight = (Muon_tightId[imu] && Muon_pfRelIso04_all[imu] < 0.15);
        imu = looseMuonIndices.at(1);
        l2IsTight = (Muon_tightId[imu] && Muon_pfRelIso04_all[imu] < 0.15);
        imu = looseMuonIndices.at(2);
        l3IsTight = (Muon_tightId[imu] && Muon_pfRelIso04_all[imu] < 0.15);
     }
}

// Always ordered: 0 - Zlep1, 1 - Zlep2, 2 - Wlep (lep IDs are clear from channel)
void WZSelectorBase::SetGoodLeptonsFromNano() {
    leptons.clear();
    bool zee = (channel_ == eem || channel_ == eee);
    auto& indices = zee ? looseElecIndices : looseMuonIndices;
    if (!(indices.size() == 2 || indices.size() == 3)) {
        throw std::length_error("Invalid lepton indices");
    }
    for (const auto& i : indices) {
        auto lep = zee ?
            LorentzVector(Electron_pt[i], Electron_eta[i], Electron_phi[i], Electron_mass[i]) :
            LorentzVector(Muon_pt[i], Muon_eta[i], Muon_phi[i], Muon_mass[i]);
        leptons.push_back(lep);
    }

    if (channel_ == eem || channel_ == emm) {
        auto& wIndices = (channel_ == eem) ? looseMuonIndices : looseElecIndices;
        if (wIndices.size() != 1)
            throw std::length_error("Invalid W lepton indices");
        size_t wi = wIndices.at(0);
        auto wlep = (channel_ == eem) ?
                LorentzVector(Muon_pt[wi], Muon_eta[wi], Muon_phi[wi], Muon_mass[wi]) :
                LorentzVector(Electron_pt[wi], Electron_eta[wi], Electron_phi[wi], Electron_mass[wi]);
        leptons.push_back(wlep);
    }
}

void WZSelectorBase::SetChannelAndIndicesNano() {
    // cut-based ID Fall17 V2 (0:fail, 1:veto, 2:loose, 3:medium, 4:tight)
    nCBVIDTightElec = 0;
    nCBVIDVetoElec = 0;
    nWZTightMuon = 0;
    nWZMediumMuon = 0;

    //TODO: Embed these variables in the NanoSkims
    looseMuonIndices.clear();
    looseElecIndices.clear();
    for (size_t i = 0; i < nMuon; i++) {
        if (Muon_tightId[i] && Muon_pfRelIso04_all[i] < 0.40) {
            nWZMediumMuon++;
            looseMuonIndices.push_back(i);
            nWZTightMuon += (Muon_pfRelIso04_all[i] < 0.15);
        }
    }

    for (size_t i = 0; i < nElectron; i++) {
        if (Electron_cutBased[i] >= 1) {
            nCBVIDVetoElec++;
            looseElecIndices.push_back(i);
        }
        if (Electron_cutBased[i] == 4)
            nCBVIDTightElec++;
    }

    if (nWZMediumMuon == 0 && nCBVIDVetoElec == 3)
        channelName_ = "eee";
    else if (nWZMediumMuon == 1 && nCBVIDVetoElec == 2)
        channelName_ = "eem";
    else if (nWZMediumMuon == 2 && nCBVIDVetoElec == 1)
        channelName_ = "emm";
    else if (nWZMediumMuon == 3 && nCBVIDVetoElec == 0)
        channelName_ = "mmm";
    else
        channelName_ = "Unknown";
    
    //std::cout << "Channel " << channelName_ << " " << channel_ << " elecIndices " << looseElecIndices.size() 
    //          << " muon indices " << looseMuonIndices.size() << std::endl;

    channel_ = channelMap_[channelName_];
}

void WZSelectorBase::LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation){ 
    weight = 1;
    b_l1Pt->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l2Eta->GetEntry(entry);
    b_l3Eta->GetEntry(entry);

    if (isMC_) {
        b_genWeight->GetEntry(entry);
        b_l1GenPt->GetEntry(entry);
        b_l2GenPt->GetEntry(entry);
        b_l3GenPt->GetEntry(entry);
        b_nTruePU->GetEntry(entry);
        ApplyScaleFactors();
    }
    else {
        b_Flag_duplicateMuonsPass->GetEntry(entry);          
        b_Flag_badMuonsPass->GetEntry(entry);          
    }
    b_ZMass->GetEntry(entry);
    b_l1IsTight->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l3MtToMET->GetEntry(entry);
    b_MET->GetEntry(entry);
    b_nCBVIDTightElec->GetEntry(entry);
    b_nCBVIDHLTSafeElec->GetEntry(entry);
    b_nCBVIDVetoElec->GetEntry(entry);
    b_nWZTightMuon->GetEntry(entry);
    b_nWZMediumMuon->GetEntry(entry);
    b_Flag_BadPFMuonFilterPass->GetEntry(entry);                    
    b_Flag_BadChargedCandidateFilterPass->GetEntry(entry);          
    b_Flag_HBHENoiseFilterPass->GetEntry(entry);                    
    b_Flag_HBHENoiseIsoFilterPass->GetEntry(entry);                 
    b_Flag_EcalDeadCellTriggerPrimitiveFilterPass->GetEntry(entry); 
    b_Flag_goodVerticesPass->GetEntry(entry);                       
    b_Flag_eeBadScFilterPass->GetEntry(entry);                      
    b_Flag_globalTightHalo2016FilterPass->GetEntry(entry);          

    // Veto on tight leptons
    // Make sure tight leptons also pass loose
    // passesLeptonVeto = nWZTightMuon + nCBVIDTightElec <= 3 &&
    //                      (nWZMediumMuon + nCBVIDHLTSafeElec) >= (nWZTightMuon+nCBVIDTightElec);
    // If tight isn't required to include loose
    // passesLeptonVeto = std::abs(nWZMediumMuon + nCBVIDHLTSafeElec - (l1IsLoose +l2IsLoose +l3IsLoose)) < 0.1;

    // Veto on loose leptons
    //passesLeptonVeto = (nWZMediumMuon + nCBVIDHLTSafeElec) == 3;
    passesLeptonVeto = (nWZMediumMuon + nCBVIDVetoElec) == 3;
 
}

void WZSelectorBase::ApplyScaleFactors() {
    weight = genWeight;
    if (channel_ == eee) {
        if (eIdSF_ != nullptr) {
            weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        if (eGsfSF_ != nullptr) {
            weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            weight *= eGsfSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
    }
    else if (channel_ == eem) {
        if (eIdSF_ != nullptr) {
            weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        }
        if (eGsfSF_ != nullptr) {
            weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        }
        if (mIdSF_ != nullptr) {
            weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        if (mIsoSF_ != nullptr) {
            weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
    }
    else if (channel_ == emm) {
        if (eIdSF_ != nullptr) {
            weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        if (eGsfSF_ != nullptr) {
            weight *= eGsfSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        if (mIdSF_ != nullptr) {
            weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        }
        if (mIsoSF_ != nullptr) {
            weight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        }
    }
    else {
        if (mIdSF_ != nullptr) {
            weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        if (mIsoSF_ != nullptr) {
            weight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
    }
    if (pileupSF_ != nullptr) {
        //weight *= pileupSF_->Evaluate1D(numPU);
        weight *= pileupSF_->Evaluate1D(nTruePU);
    }
}

void WZSelectorBase::SetMasses() {
    if (leptons.size() == 0) {
        leptons.push_back(LorentzVector(l1Pt, l1Eta, l1Phi, l1Mass));
        leptons.push_back(LorentzVector(l2Pt, l2Eta, l2Phi, l2Mass));
        leptons.push_back(LorentzVector(l3Pt, l3Eta, l3Phi, l3Mass));
    }

    ZMass = (leptons.at(0)+leptons.at(1)).M();
    Mass = (leptons.at(0)+leptons.at(1)+leptons.at(2)).M();
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool WZSelectorBase::zlep1IsTight() {
    return l1IsTight; 
}

bool WZSelectorBase::zlep2IsTight() {
    return l2IsTight; 
}

bool WZSelectorBase::tightZLeptons() {
    return zlep1IsTight() && zlep2IsTight(); 
}

bool WZSelectorBase::lepton3IsTight() {
    return l3IsTight;
}

bool WZSelectorBase::IsGenMatched3l() {
    //return true;
    return (!isMC_ || isNonpromptMC_ || 
        (isZgamma_ && l1GenPt > 0 && l2GenPt > 0) ||
        (l1GenPt > 0 && l2GenPt > 0 && l3GenPt > 0));
}
