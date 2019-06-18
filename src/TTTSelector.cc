#include "Analysis/VVAnalysis/interface/TTTSelector.h"
#include "TLorentzVector.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

// This is very WZ specific and should really be improved or likely removed
std::string TTTSelector::GetNameFromFile() {
  std::regex expr = std::regex("201[0-9]-[0-9][0-9]-[0-9][0-9]-(.*)-WZxsec2016");
  std::smatch matches;
  std::string fileName = fChain->GetTree()->GetDirectory()->GetFile()->GetName(); 

  std::regex_search(fileName, matches, expr);
  return std::string(matches.str(1));
}

void TTTSelector::SlaveBegin(TTree * /*tree*/)
{
  return;
  pileupSF_ = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
  if (pileupSF_ == nullptr ) 
    Abort("Must pass pileup weights SF");
  eIdSF_ = (ScaleFactor *) GetInputList()->FindObject("electronTightIdSF");
  if (eIdSF_ == nullptr ) 
    Abort("Must pass electron ID SF");
  eGsfSF_ = (ScaleFactor *) GetInputList()->FindObject("electronGsfSF");
  if (eGsfSF_ == nullptr ) 
    Abort("Must pass electron GSF SF");
  mIdSF_ = (ScaleFactor *) GetInputList()->FindObject("muonTightIdSF");
  if (mIdSF_ == nullptr ) 
    Abort("Must pass muon ID SF");
  mIsoSF_ = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
  if (mIsoSF_ == nullptr ) 
    Abort("Must pass muon Iso SF");

  prefireEff_ = (TEfficiency*) GetInputList()->FindObject("prefireEfficiencyMap");
  if (prefireEff_ == nullptr ) 
    Abort("Must pass prefiring efficiency map");

}

void TTTSelector::Init(TTree *tree)
{
  b.SetTree(tree);
  
  allChannels_ = {"ee", "mm", "Unknown"};
  hists1D_ = {"CutFlow", "ZMass", "ptl1", "etal1", "ptl2", "etal2"};

  SelectorBase::Init(tree);
  
}

void TTTSelector::SetBranchesNanoAOD() {
  //  NECESSARY!!!!
  b.CleanUp();

  b.SetBranch("nElectron", nElectron);
  b.SetBranch("nMuon", nMuon);
  b.SetBranch("Electron_pt", Electron_pt);
  b.SetBranch("Electron_eta", Electron_eta);
  b.SetBranch("Electron_phi", Electron_phi);
  b.SetBranch("Muon_pt", Muon_pt);
  b.SetBranch("Muon_eta", Muon_eta);
  b.SetBranch("Muon_phi", Muon_phi);
  b.SetBranch("Electron_cutBased", Electron_cutBased);
  b.SetBranch("Muon_tightId", Muon_tightId);
  b.SetBranch("Muon_tkIsoId", Muon_tkIsoId);
  b.SetBranch("Muon_pfRelIso04_all", Muon_pfRelIso04_all);
  b.SetBranch("MET_pt", MET);
  b.SetBranch("MET_phi", type1_pfMETPhi);
  b.SetBranch("Electron_charge", Electron_charge);
  b.SetBranch("Muon_charge", Muon_charge);
  b.SetBranch("Electron_mass", Electron_mass);
  b.SetBranch("Muon_mass", Muon_mass);
  if (isMC_) {
    //b.SetBranch("e1GenPt", l1GenPt, );
    //b.SetBranch("e2GenPt", l2GenPt, );
    //b.SetBranch("e3GenPt", l3GenPt, );
    b.SetBranch("genWeight", genWeight);
    b.SetBranch("Pileup_nPU", numPU);
  }
  
}

void TTTSelector::SetBranchesUWVV() {
  return;
}

void TTTSelector::LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation) {
  return;
}

void TTTSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
  weight = 1;
  b.SetEntry(entry);
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

  // cut-based ID Fall17 V2 (0:fail, 1:veto, 2:loose, 3:medium, 4:tight)
  int nCBVIDTightElec = std::count(Electron_cutBased, Electron_cutBased+nElectron, 4);
  int nCBVIDVetoElec = std::count(Electron_cutBased, Electron_cutBased+nElectron, 1);
  int nTightIdMuon = 0;
  int nMediumIdMuon = 0;

  for (size_t i = 0; i < nMuon; i++) {
    //nMediumIdMuon += (Muon_mediumId[i] && Muon_pfRelIso04_all[i] < 0.15);
    nMediumIdMuon += (Muon_mediumId[i] && Muon_pfRelIso04_all[i] < 0.15*Muon_pt[i]);
    nTightIdMuon += (Muon_tightId[i] && Muon_pfRelIso04_all[i] < 0.15);
  }


  

  channel_ = channelMap_[channelName_];
  if (nTightIdMuon >= 2) {
    channel_ = mm;
    channelName_ = "mm";
  } else if (nCBVIDTightElec >= 2) {
    channel_ = ee;
    channelName_ = "ee";
  } else {
    channel_ = Unknown;
    channelName_ = "Unknown";
  }

  // if (isMC_) {
  //   b_genWeight->GetEntry(entry);
  //   b_numPU->GetEntry(entry);
  //   //b_l1GenPt->GetEntry(entry);
  //   //b_l2GenPt->GetEntry(entry);
  //   //b_l3GenPt->GetEntry(entry);
  //   //ApplyScaleFactors();
  // }
  // else {
  //   b_Flag_duplicateMuonsPass->GetEntry(entry);          
  //   b_Flag_badMuonsPass->GetEntry(entry);          
  // }

  //  passesLeptonVeto = (nMuon + nElectron) == 3;
}


void TTTSelector::ApplyScaleFactors() {
  //  weight = genWeight;
  weight = 1;
  // skipping for now
  return;

  
  if (channel_ == eee) {
    weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
    weight *= eGsfSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
  }
  else if (channel_ == eem) {
    weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
    weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
  }
  else if (channel_ == emm) {
    weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
    weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
  }
  else {
    weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
    weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
    weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
  }
  weight *= pileupSF_->Evaluate1D(nTruePU);
}

void TTTSelector::SetShiftedMasses() {
  TLorentzVector lepton1;
  lepton1.SetPtEtaPhiM(l1Pt, l1Eta, l1Phi, l1Mass);
  TLorentzVector lepton2;
  lepton2.SetPtEtaPhiM(l2Pt, l2Eta, l2Phi, l2Mass);
  TLorentzVector lepton3;
  lepton3.SetPtEtaPhiM(l3Pt, l3Eta, l3Phi, l3Mass);
  ZMass = (lepton1+lepton2).M();
  Mass = (lepton1+lepton2+lepton3).M();
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool TTTSelector::zlep1IsTight() {
  return l1IsTight; 
}

bool TTTSelector::zlep2IsTight() {
  return l2IsTight; 
}

bool TTTSelector::tightZLeptons() {
  return zlep1IsTight() && zlep2IsTight(); 
}

bool TTTSelector::lepton3IsTight() {
  return l3IsTight;
}

bool TTTSelector::IsGenMatched3l() {
  //return true;
  return (!isMC_ || isNonpromptMC_ || 
	  (isZgamma_ && l1GenPt > 0 && l2GenPt > 0) ||
	  (l1GenPt > 0 && l2GenPt > 0 && l3GenPt > 0));
}


void TTTSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
  int step = 0;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // if (channel_ != mm && channel_ != ee) 
  //   return;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // if (!passesTrigger)
  //   return;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // if (channel_ == ee && (std::abs(l1Eta) > 2.4 || std::abs(l2Eta) > 2.4 ))
  //   return;
  // else if (channel_ == mm && (std::abs(l1Eta) > 2.5 || std::abs(l2Eta) > 2.5 ))
  //   return;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // if (l1Pt < 25 || l2Pt < 25)
  //   return;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // if (ZMass > 116.1876 || ZMass < 76.1876)
  //   return;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // if (MET > 25)
  //   return;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // if (!tightZLeptons())
  //   return;
  // SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  SafeHistFill(histMap1D_, getHistName("ZMass", variation.second), ZMass, weight);
  SafeHistFill(histMap1D_, getHistName("ptl1", variation.second), Muon_pt[0], weight);
  SafeHistFill(histMap1D_, getHistName("ptl2", variation.second), Muon_pt[1], weight);


}

void TTTSelector::SetupNewDirectory() {
  SelectorBase::SetupNewDirectory();

  InitializeHistogramsFromConfig();
}

