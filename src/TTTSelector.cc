#include "Analysis/VVAnalysis/interface/TTTSelector.h"
#include "TLorentzVector.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

#define Fill1D(NAME, VALUE_) HistFullFill(histMap1D_, NAME, variation.second, VALUE_, weight);

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
  b.SetBranch("Electron_charge", Electron_charge);
  b.SetBranch("Electron_mass", Electron_mass);
  b.SetBranch("Electron_mass", Electron_miniPFRelIso_all);
  b.SetBranch("Electron_miniPFRelIso_all", Electron_miniPFRelIso_all);
  // b.SetBranch("Electron_dxy", Electron_dxy);
  // b.SetBranch("Electron_dz", Electron_dz);
  // b.SetBranch("Electron_sip3d", Electron_sip3d);
  if(year_ == yr2018) {
    b.SetBranch("Electron_mvaFall17V2noIso", Electron_MVA);
    b.SetBranch("Electron_cutBased", Electron_cutBased);
  } else if(year_ == yr2017) {
    b.SetBranch("Electron_mvaFall17V1noIso", Electron_MVA);
    b.SetBranch("Electron_cutBased_Fall17_V1", Electron_cutBased);
  } else if(year_ == yr2016 || year_ == yrdefault) {
    b.SetBranch("Electron_mvaSpring16GP", Electron_MVA);
    b.SetBranch("Electron_cutBased_Sum16", Electron_cutBased);
  }

  b.SetBranch("nMuon", nMuon);
  b.SetBranch("Muon_pt", Muon_pt);
  b.SetBranch("Muon_eta", Muon_eta);
  b.SetBranch("Muon_phi", Muon_phi);
  b.SetBranch("Muon_tightId", Muon_tightId);
  b.SetBranch("Muon_mediumId", Muon_mediumId);
  b.SetBranch("Muon_tkIsoId", Muon_tkIsoId);
  b.SetBranch("Muon_pfRelIso04_all", Muon_pfRelIso04_all);
  b.SetBranch("Muon_miniPFRelIso_all", Muon_miniPFRelIso_all);
  b.SetBranch("Muon_charge", Muon_charge);
  b.SetBranch("Muon_mass", Muon_mass);

  b.SetBranch("nJet", nJet);
  b.SetBranch("Jet_btagCSVV2", Jet_btagCSVV2);
  b.SetBranch("Jet_btagDeepB", Jet_btagDeepB);
  b.SetBranch("Jet_eta", Jet_eta);
  b.SetBranch("Jet_phi", Jet_phi);
  b.SetBranch("Jet_pt", Jet_pt);
  b.SetBranch("Jet_mass", Jet_mass);

  b.SetBranch("Jet_neHEF", Jet_neHEF);
  b.SetBranch("Jet_neEmEF", Jet_neEmEF);
  b.SetBranch("Jet_nConstituents", Jet_nConstituents);
  b.SetBranch("Jet_chHEF", Jet_chHEF);
  b.SetBranch("Jet_chEmEF", Jet_chEmEF);

  b.SetBranch("MET_pt", MET);
  b.SetBranch("MET_phi", type1_pfMETPhi);
  
  if (isMC_) {
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

/// Make to seperate fuctionality
void TTTSelector::clearValues() {
  weight = 1;
  HT = 0;
  nTightJet = 0;
  nBJets = 0;
  goodParts.clear();
}

void TTTSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
  clearValues();
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

  /////////////////////
  // Setup Electrons //
  /////////////////////
    
  for (size_t i = 0; i < nElectron; i++) {
    // if(IsGoodMVAElectron2016(i)) {
    if(IsGoodElectron(i)) {
      // // Extra Iso requirement
      // TLorentzVector lep;
      // lep.SetPtEtaPhiM(Electron_pt[i], Electron_eta[i], Electron_phi[i], Electron_mass[i]);
      // if(!passFullIso(lep, 0.8, 7.2)) continue;
      // Setup goodPart
      //      if(Electron_dxy[i] > 0.05) std::cout << "here" << std::endl;
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
    if(IsGoodMuon(i)) {
      // // Extra Iso requirement
      // TLorentzVector lep;
      // lep.SetPtEtaPhiM(Muon_pt[i], Muon_eta[i], Muon_phi[i], Muon_mass[i]);
      // if(!passFullIso(lep, 0.76, 7.2)) continue;
      // Setup goodPart
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

    //common to both jets, may put more here
    if(!isLooseJetId(i)) continue;

    // regular jets
    if(IsGoodJet(i)) {
      nTightJet++;
      HT += Jet_pt[i];
    }

    // bjet 
    if(IsGoodBJet(i)) nBJets++;
  }
  
  channel_ = channelMap_[channelName_];
  if(goodParts.size() == 2) {
    channel_ = Unknown;
    channelName_ = "Unknown";
  } else if(goodParts[0].ptype == pType::Muon && goodParts[1].ptype == pType::Muon) {
    channel_ = mm;
    channelName_ = "mm";
  } else if(goodParts[0].ptype == pType::Electron && goodParts[1].ptype == pType::Electron) {
    channel_ = ee;
    channelName_ = "ee";
  } else {
    channel_ = em;
    channelName_ ="em";
    /// fix order of leptons by pt
    if(goodParts[0].v.Pt() < goodParts[1].v.Pt()) {
      std::swap(goodParts[0], goodParts[1]);
    }
  }

  if (isMC_) {
    ApplyScaleFactors();
  }
}


void TTTSelector::ApplyScaleFactors() {
  weight *= (genWeight > 0) ? 1.0 : -1.0;
  //  weight = 1;
  // skipping for now
  return;
  //// to add!!
}

////////////////////////////////////////////////////
// Functions for defining particles: used in main //
// loop of leptons/jets for getting multiplicity  //
////////////////////////////////////////////////////

bool TTTSelector::IsGoodMuon(size_t index) {
  return ( (Muon_pt[index] > 20) &&
	   (abs(Muon_eta[index]) < 2.4) &&
	   Muon_mediumId[index] &&
	   (Muon_miniPFRelIso_all[index] < 0.16) );
		       }

bool TTTSelector::IsGoodCBElectron(size_t index) {
  return ((Electron_pt[index] > 20) &&
	  (abs(Electron_eta[index]) < 2.5) &&
	  (Electron_miniPFRelIso_all[index] < 0.12) &&
	  (Electron_cutBased[index] == 4));
}

bool TTTSelector::IsGoodMVAElectron2016(size_t index) {
  bool mvaRec = false;
  if(abs(Electron_eta[index]) < 0.8)
    mvaRec = Electron_MVA[index] > std::max(0.52, 0.77-0.025*(Electron_pt[index]-15));
  else if(abs(Electron_eta[index]) < 1.479)
    mvaRec = Electron_MVA[index] > std::max(0.11, 0.56-0.045*(Electron_pt[index]-15));
  else if(abs(Electron_eta[index]) < 2.5)
    mvaRec = Electron_MVA[index] > std::max(-0.01, 0.48-0.049*(Electron_pt[index]-15));

  return ((Electron_pt[index] > 20) &&
	  (Electron_miniPFRelIso_all[index] < 0.12) &&
	  mvaRec);
}

bool TTTSelector::IsGoodMVAElectron2017(size_t index) {
  bool mvaRec = false;
  if(abs(Electron_eta[index]) < 0.8)
    mvaRec = std::max(0.52, 0.77-0.025*(Electron_pt[index]-15));
  else if(abs(Electron_eta[index]) < 1.479)
    mvaRec = std::max(0.11, 0.56-0.045*(Electron_pt[index]-15));
  else if(abs(Electron_eta[index]) < 2.5)
    mvaRec = std::max(-0.01, 0.48-0.049*(Electron_pt[index]-15));

  return ((Electron_pt[index] > 20) &&
	  mvaRec);
}


bool TTTSelector::IsGoodJet(size_t index) {
  return ((Jet_pt[index] > 40.0) &&
	  (abs(Jet_eta[index]) < 2.4) &&
	  isOverlap(index)
	  );
}

/// TODO: add toggle for different btag stuff
bool TTTSelector::IsGoodBJet(size_t index) {
  return ((Jet_pt[index] > 25.0) &&
	  (abs(Jet_eta[index]) < 2.4) &&
	  (Jet_btagCSVV2[index] > 0.8484) &&  
	  //(Jet_btagDeepB[index] > 0.6324) &&
	  isOverlap(index)
	  );
}

bool TTTSelector::isLooseJetId(size_t index) {
  return (Jet_neHEF[index] < 0.99 &&
	  Jet_neEmEF[index] < 0.99 &&
	  Jet_nConstituents[index] > 1 &&
	  Jet_chHEF[index] > 0 &&
	  Jet_chEmEF[index] < 0.99
	  );
}

bool TTTSelector::isTightJetId(size_t index) {
  return (Jet_neHEF[index] < 0.9 &&
	  Jet_neEmEF[index] < 0.9 &&
	  Jet_nConstituents[index] > 1 &&
	  Jet_chHEF[index] > 0
	  );
}

bool TTTSelector::passFullIso(TLorentzVector& lep, int I2, int I3) {
  TLorentzVector jet, closeJet;
  double minDR = 10;
  for(size_t index = 0; index < nJet; index++) {
    jet.SetPtEtaPhiM(Jet_pt[index], Jet_eta[index], Jet_phi[index], Jet_mass[index]);
    if(minDR > lep.DeltaR(jet)) {
      closeJet = jet;
      minDR = lep.DeltaR(jet);
    }
  }
  return ((lep.Pt()/closeJet.Pt() > I2 ) ||
	  (lep.Pt()*std::sin(lep.Angle(closeJet.Vect())) > I3)
	  );
}

bool TTTSelector::isOverlap(size_t index) {
  TLorentzVector tmp;
  double dR = 0.4;
  tmp.SetPtEtaPhiM(Jet_pt[index], Jet_eta[index], Jet_phi[index], Jet_mass[index]);
  return ((tmp.DeltaR(goodParts[0].v) > dR) &&
	  (tmp.DeltaR(goodParts[1].v) > dR));
}

void TTTSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
  int step = 0;
  Fill1D("CutFlow", 0);
  
  /// 2 good leptons
  if(goodParts.size() != 2) return;
  Fill1D("CutFlow", ++step);

  // first lep requirement
  if(goodParts[0].v.Pt() < 25) return;
  Fill1D("CutFlow", ++step);
  
  // same sign requirement
  if(goodParts[0].charge * goodParts[1].charge != 1) return;
  Fill1D("CutFlow", ++step);

  // met cut
  if (MET < 50) return;
  Fill1D("CutFlow", ++step);

  // ht cut
  if(HT < 300 ) return;
  Fill1D("CutFlow", ++step);
  
  // jet cut
  if(nTightJet < 4) return;
  Fill1D("CutFlow", ++step);
  
  // bjet cut
  if(nBJets < 2) return;
  Fill1D("CutFlow", ++step);
  
  // // veto cut
  // if(!passesLeptonVeto)
  //   Fill1D("CutFlow", ++step);
  // in SR stuff

  // met cut
  if (MET < 50) return;
  SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

  Fill1D("ptl1", goodParts[0].Pt());
  Fill1D("ptl2", goodParts[1].Pt());
  Fill1D("SR", getSRBin());
  Fill1D("njet", nTightJet);
  Fill1D("nbjet", nBJets);
  
  for(size_t i = 0; i < nJet; i++) {
    if(IsGoodJet(i)) {
      Fill1D("jetpt", Jet_pt[i]);
    }
    if(IsGoodBJet(i)) {
      Fill1D("bjetpt", Jet_pt[i]);
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
    else if(nTightJet == 6) return 4;
    else if(nTightJet == 7) return 5;
    else if(nTightJet >= 8) return 5;
  } else if(nBJets >= 4) {
    if(nTightJet >= 5)     return 6;
  }
  return -1;

  // if(nBJets == 2) {
  //   if(nTightJet <= 5)      return 0;
  //   if(nTightJet == 6)      return 1;
  //   else if(nTightJet == 7) return 2;
  //   else if(nTightJet >= 8) return 3;
  // } else if(nBJets == 3) {
  //   if(nTightJet == 5)      return 4;
  //   else if(nTightJet == 6) return 5;
  //   else if(nTightJet == 7) return 6;
  //   else if(nTightJet >= 8) return 7;
  // } else if(nBJets >= 4) {
  //   if(nTightJet >= 5)     return 8;
  // }
  // return -1;

  
}

