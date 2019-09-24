
#include "Analysis/VVAnalysis/interface/Efficiency.h"
#include "TLorentzVector.h"
#define Fill1D(NAME, VALUE_) HistFullFill(histMap1D_, NAME, variation.second, VALUE_, weight_g);

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>>LorentzVector;

enum PID {PID_MUON = 13, PID_ELECTRON = 11, PID_BJET = 5};
enum TYPE {RECO =1, FAKE = 2, NOT_RECO = 3};

void Efficiency::Init(TTree *tree) {

    hists1D_ = {"GenElecPt", "GenMuonPt", "MatchElecPt", "MatchMuonPt",
		"FakeElecPt", "FakeMuonPt", "NGenElec", "NGenMuon", };
    b.SetTree(tree);

    allChannels_ = {"all"};
    
    SelectorBase::Init(tree);
    
    TList* slaveClassList = GetInputList();
    slaveClassList->Add(new TNamed("isSlaveClass", "isSlaveClass"));
    TTTAna.SetInputList(slaveClassList);
    TTTAna.Init(tree);
    TTTAna.SetBranches();
    std::cout << "start" << "\n";
}

void Efficiency::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}

void Efficiency::SetBranchesNanoAOD() {
    std::cout << "begin" << "\n";
    b.CleanUp();
    
    b.SetBranch("nGenPart", nGenPart);
    b.SetBranch("GenPart_pt", GenPart_pt);
    b.SetBranch("GenPart_pdgId", GenPart_pdgId);
    b.SetBranch("GenPart_eta", GenPart_eta);
    b.SetBranch("GenPart_phi", GenPart_phi);
    b.SetBranch("GenPart_mass",GenPart_mass);
    b.SetBranch("GenPart_genPartIdxMother", GenPart_statusFlags);
    std::cout << "before this" << "\n";
}

void Efficiency::clearValues() {
    weight_g=1;
    Leptons.clear();
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
	    if (GenPart_pt[i]< 5 || abs(GenPart_pdgId[GenPart_statusFlags[i]]) != 24) continue;
	    Leptons.push_back(GenPart());
	    Leptons.back().SetupGen(GenPart_pt[i], GenPart_eta[i], GenPart_phi[i], GenPart_mass[i],
				    GenPart_pdgId[i]);
	    //}

	}
    }

    // std::cout << "gen" << "\n";
    // for(auto lep: Leptons) {
    //     if(lep.gId() == PID_MUON) std::cout << lep.gPt() << " " << lep.gEta()  << "\n";
    // }
    // std::cout << "reco" << "\n";
    // for(auto lep: TTTAna.goodLeptons) {
    //     if(lep.Id() == PID_MUON) std::cout << lep.Pt() << " " << lep.Eta()  << "\n";
    // }
    
    for(auto reco : TTTAna.goodLeptons) {
	bool isMatched = false;
	for(auto& gen : Leptons) {
	    if (gen.gId() == reco.Id() && reco::deltaR(gen.gVector(), reco.v)<0.05) {
		gen.SetupReco(reco);
		isMatched = true;
		break;
	    }
	}
	if(!isMatched) {
	    Leptons.push_back(GenPart());
	    Leptons.back().SetupReco(reco);
	}
    }

    weight_g = TTTAna.weight;
    
    channelName_ = "all";
    channel_ = channelMap_[channelName_];
}

void Efficiency::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) {
    int nGenElec = 0;
    int nGenMuon = 0;
    int nRecoElec = 0;
    int nRecoMuon = 0;
      
    for (auto part: Leptons) {
        if (part.gId() == PID_ELECTRON || part.rId() == PID_ELECTRON) {

	    if(part.isMatched()) {
		Fill1D("MatchElecPt", part.gPt());
	    } else if(part.isFaked()) {
		Fill1D("FakeElecPt", part.rPt());
	    }
	
	    Fill1D("GenElecPt", part.gPt());
	    nGenElec ++;
	} else if (part.gId() == PID_MUON || part.rId() == PID_MUON) {
	    if(part.isMatched()) {
		Fill1D("MatchMuonPt", part.gPt());
	    } else if(part.isFaked()) {
		Fill1D("FakeMuonPt", part.rPt());
	    }
	
	    Fill1D("GenMuonPt", part.gPt());
	    nGenMuon ++;
	}
	
                        
    }
  
    Fill1D("NGenElec", nGenElec);
    Fill1D("NGenMuon", nGenMuon);
    // Fill1D("nRecoElec", nRecoElec);
    // Fill1D("nRecoMuon", nRecoMuon);
    return;
}



