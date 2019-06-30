#include "Analysis/VVAnalysis/interface/TTTSelector.h"
#include "TLorentzVector.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

#define FullFill(NAME, VALUE_) SafeHistFill(histMap1D_, NAME, VALUE_, weight);

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
  
  allChannels_ = {"ee", "mm", "em", "all"};
  hists1D_ = {"CutFlow", "ZMass", "ptl1", "etal1", "ptl2", "etal2", "SR", "bjetpt", "jetpt", "nbjet", "njet"};

  SelectorBase::Init(tree);
  
}

void TTTSelector::SetBranchesNanoAOD() {
  //  NECESSARY!!!!
  b.CleanUp();

  b.SetBranch("nElectron", nElectron);
  b.SetBranch("Electron_pt", Electron_pt);
  b.SetBranch("Electron_eta", Electron_eta);
  b.SetBranch("Electron_phi", Electron_phi);
  b.SetBranch("Electron_cutBased", Electron_cutBased);
  b.SetBranch("Electron_charge", Electron_charge);
  b.SetBranch("Electron_mass", Electron_mass);

  b.SetBranch("nMuon", nMuon);
  b.SetBranch("Muon_pt", Muon_pt);
  b.SetBranch("Muon_eta", Muon_eta);
  b.SetBranch("Muon_phi", Muon_phi);
  b.SetBranch("Muon_tightId", Muon_tightId);
  b.SetBranch("Muon_mediumId", Muon_mediumId);
  b.SetBranch("Muon_tkIsoId", Muon_tkIsoId);
  b.SetBranch("Muon_pfRelIso04_all", Muon_pfRelIso04_all);
  b.SetBranch("Muon_miniPFRnelIso_all", Muon_miniPFRelIso_all);
  b.SetBranch("Muon_charge", Muon_charge);
  b.SetBranch("Muon_mass", Muon_mass);

  b.SetBranch("nJet", nJet);
  b.SetBranch("Jet_btagCSVV2", Jet_btagCSVV2);
  b.SetBranch("Jet_btagDeepB", Jet_btagDeepB);
  b.SetBranch("Jet_eta", Jet_eta);
  b.SetBranch("Jet_phi", Jet_phi);
  b.SetBranch("Jet_pt", Jet_pt);
  b.SetBranch("Jet_mass", Jet_mass);
  
  b.SetBranch("MET_pt", MET);
  b.SetBranch("MET_phi", type1_pfMETPhi);
  
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
  HT = 0;
  nTightJet = 0;
  nBJets = 0;

  
  b.SetEntry(entry);
  goodParts.clear();
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
  int nCBVIDTightElec = 0;
  int nCBVIDVetoElec = std::count(Electron_cutBased, Electron_cutBased+nElectron, 1);
  int nTightIdMuon = 0;
  int nMediumIdMuon = 0;
  
  
  /////////////////////
  // Setup Electrons //
  /////////////////////
    
  for (size_t i = 0; i < nElectron; i++) {
    if(IsTightElectron(i)) {
      nCBVIDTightElec++;
      goodParts.push_back(GoodPart());
      goodParts.back().SetTVector(Electron_pt[i], Electron_eta[i], Electron_phi[i], Electron_mass[i]);
      goodParts.back().SetpType(pType::Electron);
      goodParts.back().SetCharge(Electron_charge[i]);
    }
  }

  /////////////////
  // Setup Muons //
  /////////////////
  
  for (size_t i = 0; i < nMuon; i++) {
    nMediumIdMuon += (Muon_mediumId[i] && Muon_pfRelIso04_all[i] < 0.15*Muon_pt[i]);
    if(IsTightMuon(i)) {
      nTightIdMuon++;
      goodParts.push_back(GoodPart());
      goodParts.back().SetTVector(Muon_pt[i], Muon_eta[i], Muon_phi[i], Muon_mass[i]);
      goodParts.back().SetpType(pType::Muon);
      goodParts.back().SetCharge(Muon_charge[i]);
    }
  }

  ////////////////
  // Setup Jets //
  ////////////////
  for(size_t i = 0; i < nJet; i++) {
    if(goodParts.size() != 2) break;
    // regular jets
    if(IsTightJet(i)) {
      nTightJet++;
      HT += Jet_pt[i];
    }
    // add tight jet requirement with branch added in time and continue statements 
    
    // bjet 
    if(IsTightBJet(i)) {
      nBJets++;
    }
  }
  
 
  channel_ = channelMap_[channelName_];
  if (nTightIdMuon == 2) {
    channel_ = mm;
    channelName_ = "mm";

  } else if (nCBVIDTightElec == 2) {
    channel_ = ee;
    channelName_ = "ee";
  } else if((nTightIdMuon + nCBVIDTightElec) == 2) {
    channel_ = em;
    channelName_ ="em";
    /// fix order of leptons by pt
    if(goodParts[0].v.Pt() < goodParts[1].v.Pt()) {
      std::swap(goodParts[0], goodParts[1]);
    }
  } else {
    channel_ = Unknown;
    channelName_ = "Unknown";
  }

  if (isMC_) {
    ApplyScaleFactors();
  }

  //// need veto stuff
  //  passesLeptonVeto = (nMuon + nElectron) == 3;
}


void TTTSelector::ApplyScaleFactors() {
  weight *= (genWeight > 0) ? 1.0 : -1.0;
  //  weight = 1;
  // skipping for now
  return;
  //// to add!!
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool TTTSelector::IsTightMuon(size_t index) {
  return ( (Muon_pt[index] > 20) &&
	   (abs(Muon_eta[index]) < 2.4) &&
	   Muon_mediumId[index] &&
	   (Muon_miniPFRelIso_all[index] < 0.16) );
}

bool TTTSelector::IsTightElectron(size_t index) {
  return ((Electron_pt[index] > 20) &&
	  (abs(Electron_eta[index]) < 2.5) &&
	  (Electron_cutBased[index] == 4));
}

bool TTTSelector::IsTightJet(size_t index) {
  return ((Jet_pt[index] > 40) &&
	  (abs(Jet_eta[index]) < 2.4) &&
	  IsOverlap(index));
}

bool TTTSelector::IsTightBJet(size_t index) {
  return ((Jet_pt[index] > 25) &&
	  (abs(Jet_eta[index]) < 2.4) &&
	  (Jet_btagCSVV2[index] > 0.8484) &&
	  IsOverlap(index));
}

bool TTTSelector::IsOverlap(size_t index) {
  return true;
  TLorentzVector tmp;
  double dR = 0.4;
  tmp.SetPtEtaPhiM(Jet_pt[index], Jet_eta[index], Jet_phi[index], Jet_mass[index]);
  return ((tmp.DeltaR(goodParts[0].v) > dR) &&
	  (tmp.DeltaR(goodParts[1].v) > dR));
}

void TTTSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
  int step = 0;
  SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);
  SafeHistFill(histMap1D_, "CutFlow_all", 0, weight);
  
  /// 2 good leptons
  if(goodParts.size() != 2) return;
  SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // first lep requirement
  if(goodParts[0].v.Pt() < 25) return;
  SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);
  
  // same sign requirement
  if(goodParts[0].charge * goodParts[1].charge != 1) return;
  SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // met cut
  if (MET < 50) return;
  SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  // ht cut
  if(HT < 300 ) return;
  SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);
  
  // jet cut
  if(nTightJet < 4) return;
  
  // bjet cut
  if(nBJets < 2) return;
  
  // veto cut

  // in SR stuff
  SafeHistFill(histMap1D_, "CutFlow_all", 1, weight);

  FullFill(getHistName("ptl1", variation.second), goodParts[0].v.Pt());
  FullFill("ptl1_all", goodParts[0].v.Pt());

  FullFill(getHistName("ptl2", variation.second), goodParts[1].v.Pt());
  FullFill("ptl2_all", goodParts[1].v.Pt());
  
  FullFill(getHistName("SR", variation.second), getSRBin());
  FullFill("SR_all", getSRBin());

  FullFill(getHistName("njet", variation.second), nTightJet);
  FullFill("njet_all", nTightJet);

  FullFill(getHistName("nbjet", variation.second), nBJets);
  FullFill("nbjet_all", nBJets);

  
  for(size_t i = 0; i < nJet; i++) {
    if(IsTightJet(i)) {
      FullFill(getHistName("jetpt", variation.second), Jet_pt[i]);
      FullFill("jetpt_all", Jet_pt[i]);
    }
    if(IsTightBJet(i)) {
      FullFill(getHistName("bjetpt", variation.second), Jet_pt[i]);
      FullFill("bjetpt_all", Jet_pt[i]);
    }
  }
}

void TTTSelector::SetupNewDirectory() {
  SelectorBase::SetupNewDirectory();

  InitializeHistogramsFromConfig();
}

int TTTSelector::getSRBin() {
  if(nBJets == 2) {
    if(nTightJet <= 5)      return 0;
    if(nTightJet == 6)      return 1;
    else if(nTightJet == 7) return 2;
    else if(nTightJet >= 8) return 3;
  } else if(nBJets == 3) {
    if(nTightJet == 5)      return 4;
    else if(nTightJet == 6) return 5;
    else if(nTightJet == 7) return 6;
    else if(nTightJet >= 8) return 7;
  } else if(nBJets >= 4) {
    if(nTightJet >= 5)     return 8;
  }
  return -1;
}

