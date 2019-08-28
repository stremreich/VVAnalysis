#include "Analysis/VVAnalysis/interface/Efficiency.h"
#include "TLorentzVector.h"
#define Fill1D(NAME, VALUE_) HistFullFill(histMap1D_, NAME, variation.second, VALUE_, weight_g);
#define SetupPtEtaPhiM(PART, INDEX) PART##_pt[INDEX], PART##_eta[INDEX], PART##_phi[INDEX], PART##_mass[INDEX]

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>>LorentzVector;

void Efficiency::Init(TTree *tree) {
  
    b.SetTree(tree);

    allChannels_ = {"mm", "ee", "em", "all", "lll"};

    hists1D_ = {"GenMuonPt","GenElecPt", "nGenMuon", "nGenElec", "nFakeMuon", "nFakeElec", "nRecoElec", "nRecoMuon", "RecoMuonPt", "RecoElecPt"};
   
    // hists2D_ = {"bJetvsJets",    "Beff_b_btag", "Beff_j_btag", "Beff_b", "Beff_j"};

    SelectorBase::Init(tree);
    
    // for (auto item : *GetInputList()) {
    // std::cout<< item->GetTitle() <<std::endl;
    // }
    
    // TList* histInfo = (TList* ) GetInputList()->FindObject("histinfo");
    // for (auto && entry : *histInfo) {  
    //   TNamed* currentHistInfo = dynamic_cast<TNamed*>(entry);
    //   std::string name = currentHistInfo->GetName();
    //   std::cout << name<<std::endl;
    // }
    
    TTTAna.SetInputList(GetInputList());
    TTTAna.Init(tree);
}
void Efficiency::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}

void Efficiency::SetBranchesNanoAOD() {
  b.CleanUp();
  // TTTAna.SetBranchesNanoAOD();
  b.SetBranch("nGenPart", nGenPart);
  b.SetBranch("GenPart_pt", GenPart_pt);
  b.SetBranch("GenPart_pdgId", GenPart_pdgId);
  b.SetBranch("GenPart_eta", GenPart_eta);
  b.SetBranch("GenPart_phi", GenPart_phi);
  b.SetBranch("GenPart_mass",GenPart_mass);

  
}

void Efficiency::clearValues() {
  weight_g=1;
  genElec.clear();
  genMuon.clear();
  recoElec.clear();
  recoMuon.clear();
  fakeElec.clear();
  fakeMuon.clear();
}

void Efficiency::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) {
  clearValues();
  b.SetEntry(entry);
  TTTAna.LoadBranchesNanoAOD(entry,variation);

  for(size_t i =0; i< nGenPart; i++) {
    if (abs(GenPart_pdgId[i]) == Electron) {
      genElec.push_back(GenPart(SetupPtEtaPhiM(GenPart,i)));
      
      
    }
    else if (abs(GenPart_pdgId[i]) == Muon) {
      genMuon.push_back(GenPart(SetupPtEtaPhiM(GenPart,i)));
    }
  }
  
  int fakeIdx=0;
  for(auto recolep : TTTAna.goodLeptons) {
    bool found = false;
    if (abs(recolep.Id()) == Electron) {
      int eIdx =0;
      for(auto gen_elec : genElec) {
	if(reco::deltaR(recolep.v, gen_elec.v_g) <0.4) {
	  found = true;
	  recoElec.push_back(RecoPart(SetupPtEtaPhiM(TTTAna.Electron, eIdx)));
	  break;
	}
	eIdx++;
      }
    }
    else if (abs(recolep.Id()) ==Muon) {
      int mIdx =0;
      for(auto gen_muon : genMuon) {
	if (reco::deltaR(recolep.v, gen_muon.v_g)<0.4) {
	  found =true;
	  recoMuon.push_back(RecoPart(SetupPtEtaPhiM(TTTAna.Muon, mIdx)));
	  break;
	}
	mIdx++;
      }
    }
    if (!found) {
      if (abs(recolep.Id()) == Electron) {
	fakeElec.push_back(FakePart(SetupPtEtaPhiM(TTTAna.Electron, fakeIdx)));
      }
      else if (abs(recolep.Id()) ==Muon) {
	fakeMuon.push_back(FakePart(SetupPtEtaPhiM(TTTAna.Muon, fakeIdx)));
      }
      fakeIdx++;
    }
  }

  channelName_ = "all";
  channel_ = channelMap_[channelName_];
}

void Efficiency::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) {

  for (size_t x=0; x<genElec.size(); x++) {
    Fill1D("nGenElec", x);
    Fill1D("GenElecPt", genElec[x].Pt());
  }
  for (size_t y=0; y<genMuon.size(); y++) {
    Fill1D("nGenMuon", y);
    Fill1D("GenMuonPt", genMuon[y].Pt());
  }

  for (size_t a=0; a<fakeElec.size(); a++) {
    Fill1D("nFakeElec", a);
  }
  for (size_t b=0; b<fakeMuon.size(); b++) {
    Fill1D("nFakeMuon", b);
  }
  for (size_t c=0; c<recoElec.size(); c++) {
    Fill1D("nRecoElec", c);
    Fill1D("RecoElecPt", recoElec[c].Pt());
  }
  for (size_t f=0; f<recoMuon.size(); f++) {
    Fill1D("nRecoMuon", f);
    Fill1D("RecoMuonPt", recoMuon[f].Pt());
  }
  return;
}
