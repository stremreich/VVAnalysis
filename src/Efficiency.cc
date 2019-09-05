
#include "Analysis/VVAnalysis/interface/Efficiency.h"
#include "TLorentzVector.h"
#define Fill1D(NAME, VALUE_) HistFullFill(histMap1D_, NAME, variation.second, VALUE_, weight_g);
#define SetupPtEtaPhiM(PART, INDEX) PART##_pt[INDEX], PART##_eta[INDEX], PART##_phi[INDEX], PART##_mass[INDEX]

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>>LorentzVector;

enum PID {PID_MUON = 13, PID_ELECTRON = 11, PID_BJET = 5};
enum TYPE {RECO =1, FAKE = 2, NOT_RECO = 3};

void Efficiency::Init(TTree *tree) {

    hists1D_ = {"GenMuonPt","GenElecPt", "nGenMuon", "nGenElec", "nFakeMuon", "nFakeElec", "nRecoElec", "nRecoMuon", "RecoMuonPt", "RecoElecPt"};
    // hists2D_ = {"bJetvsJets",    "Beff_b_btag", "Beff_j_btag", "Beff_b", "Beff_j"};
    b.SetTree(tree);

    allChannels_ = {"all"};
    
    SelectorBase::Init(tree);
    
    TList* slaveClassList = GetInputList();
    slaveClassList->Add(new TNamed("isSlaveClass", "isSlaveClass"));
    TTTAna.SetInputList(slaveClassList);
    TTTAna.Init(tree);
    TTTAna.SetBranches();

}

void Efficiency::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}

void Efficiency::SetBranchesNanoAOD() {
    b.CleanUp();
    
    b.SetBranch("nGenPart", nGenPart);
    b.SetBranch("GenPart_pt", GenPart_pt);
    b.SetBranch("GenPart_pdgId", GenPart_pdgId);
    b.SetBranch("GenPart_eta", GenPart_eta);
    b.SetBranch("GenPart_phi", GenPart_phi);
    b.SetBranch("GenPart_mass",GenPart_mass);
  
}

void Efficiency::clearValues() {
  weight_g=1;
  //notrecoLeptons.clear();
  recoLeptons.clear();
  fakeLeptons.clear();
  genLeptons.clear();
}

void Efficiency::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) {
  clearValues();
  b.SetEntry(entry);
  TTTAna.LoadBranchesNanoAOD(entry,variation);

  if (nGenPart > N_KEEP_GEN_) {
    std::string message = "Found more Gen particles than max read number. \n Found" ;
    message += std::to_string(nGenPart);
    message += " particles\n  --> Max read number was ";
    message += std::to_string(N_KEEP_GEN_);
    message += "\nExiting because this can cause problems. Increase N_KEEP_GEN_ to avoid this error.\n";
    throw std::domain_error(message);
  }
  
  for (size_t i=0; i< nGenPart; i++) {
    if (std::abs(GenPart_pdgId[i]) == PID_ELECTRON || std::abs(GenPart_pdgId[i]) == PID_MUON) {
      genLeptons.push_back(GenPart(SetupPtEtaPhiM(GenPart, i)));
      genLeptons.back().SetPdgId(abs(GenPart_pdgId[i]));

    }
    else continue;
  }

  int idx =0;
  for (auto goodlep : TTTAna.goodLeptons) {
    bool found =false;
    for (auto genlep : genLeptons) {
      if (reco::deltaR(goodlep.v, genlep.v)<0.4) {
      // good lepton is a reco
  	if (abs(goodlep.Id()) == PID_ELECTRON) {
  	  recoLeptons.push_back(RecoPart(SetupPtEtaPhiM(TTTAna.Electron,idx)));
  	  recoLeptons.back().SetPdgId(abs(goodlep.Id()));
  	  found = true;
  	}
  	else if (abs(goodlep.Id()) == PID_MUON) {
  	  recoLeptons.push_back(RecoPart(SetupPtEtaPhiM(TTTAna.Muon, idx)));
  	  recoLeptons.back().SetPdgId(abs(goodlep.Id()));
  	  found = true;
  	}
      }
    }
    if (!found) {
      // good lepton is a fake
      if (abs(goodlep.Id()) == PID_ELECTRON) {
  	fakeLeptons.push_back(FakePart(SetupPtEtaPhiM(TTTAna.Electron, idx)));
  	fakeLeptons.back().SetPdgId(abs(goodlep.Id()));
      }
      else if (abs(goodlep.Id()) == PID_MUON) {
  	fakeLeptons.push_back(FakePart(SetupPtEtaPhiM(TTTAna.Muon, idx)));
  	fakeLeptons.back().SetPdgId(abs(goodlep.Id()));
      }
    }
    idx++;
  }

    
  channelName_ = "all";
  channel_ = channelMap_[channelName_];
}

void Efficiency::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) {
  int nGenElec = 0;
  int nGenMuon = 0;
  int nRecoElec = 0;
  int nRecoMuon = 0;
  for (auto gen: genLeptons) {
    if (gen.Id() == PID_ELECTRON) {
      Fill1D("GenElecPt", gen.Pt());
       nGenElec ++;

    }
    else if (gen.Id() == PID_MUON) {
      Fill1D("GenMuonPt", gen.Pt());
      nGenMuon ++;
	
    }
  }
  
  for (size_t x=0; x<fakeLeptons.size(); x++) {
    if (fakeLeptons[x].Id() == PID_ELECTRON) {
      Fill1D("nFakeElec", x);
    }
    else if (fakeLeptons[x].Id()==PID_MUON) {
      Fill1D("nFakeMuon", x);
    }
  }

  for (auto reco : recoLeptons) {
    if (reco.Id() == PID_ELECTRON) {
      Fill1D("RecoElecPt", reco.Pt());
      nRecoElec ++;

    }
    else if (reco.Id() == PID_MUON) {
      Fill1D("RecoMuonPt", reco.Pt());
      nRecoMuon ++;

    }
  }
  Fill1D("nGenElec", nGenElec);
  Fill1D("nGenMuon", nGenMuon);
  Fill1D("nRecoElec", nRecoElec);
  Fill1D("nRecoMuon", nRecoMuon);
  
  return;
}



