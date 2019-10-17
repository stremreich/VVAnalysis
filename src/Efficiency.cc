
#include "Analysis/VVAnalysis/interface/Efficiency.h"
#include "TLorentzVector.h"
#define Fill1D(NAME, VALUE_) HistFullFill(histMap1D_, NAME, variation.second, VALUE_, weight_g);

#define Fill2D(NAME, VALUE1_, VALUE2_) HistFullFill(histMap2D_, NAME, variation.second, VALUE1_, VALUE2_, weight_g);

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>>LorentzVector;


void Efficiency::Init(TTree *tree) {

    hists1D_ = {"GenElecPt", "GenMuonPt", "MatchElecPt", "MatchMuonPt",
		"FakeElecPt", "FakeMuonPt", "NGenElec", "NGenMuon","GenJetPt", "GenBJetPt",
		"MatchJetPt", "MatchBJetPt", "FakeJetPt", "FakeBJetPt", "NGenBJet", "NGenJet",
		"NMatchBJet", "NFakeBJet"};
    
    hists2D_ = {"BEff_b_btag", "BEff_j_btag", "GenbJetsvsJets"};
    b.SetTree(tree);

    allChannels_ = {"all"};
    
    SelectorBase::Init(tree);
    
    TList* slaveClassList = GetInputList();
    slaveClassList->Add(new TNamed("isSlaveClass", "isSlaveClass"));
    TTTAna.SetInputList(slaveClassList);
    TTTAna.Init(tree);
    TTTAna.SetBranches();
    // std::cout << "start" << "\n";
}

void Efficiency::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}

void Efficiency::SetBranchesNanoAOD() {
  // std::cout << "begin" << "\n";
    b.CleanUp();
    
    b.SetBranch("nGenPart", nGenPart);
    b.SetBranch("GenPart_pt", GenPart_pt);
    b.SetBranch("GenPart_pdgId", GenPart_pdgId);
    b.SetBranch("GenPart_eta", GenPart_eta);
    b.SetBranch("GenPart_phi", GenPart_phi);
    b.SetBranch("GenPart_mass",GenPart_mass);
    b.SetBranch("GenPart_genPartIdxMother", GenPart_statusFlags);

    b.SetBranch("nGenJet", nGenJet);
    b.SetBranch("GenJet_eta", GenJet_eta);
    b.SetBranch("GenJet_phi", GenJet_phi);
    b.SetBranch("GenJet_pt", GenJet_pt);
    b.SetBranch("GenJet_mass", GenJet_mass);
    b.SetBranch("GenJet_partonFlavour", GenJet_partonFlavour);
  

    
    // std::cout << "before this" << "\n";
}

void Efficiency::clearValues() {
    weight_g=1;
    Leptons.clear();
    Jets.clear();
}

void Efficiency::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) {
    clearValues();
    b.SetEntry(entry);
    
    TTTAna.LoadBranchesNanoAOD(entry,variation);

    if (nGenJet > N_KEEP_GEN_JET) {
	std::string message = "Found more Gen jets than max read number. \n Found" ;
	message += std::to_string(nGenJet);
	message += " particles\n  --> Max read number was ";
	message += std::to_string(N_KEEP_GEN_JET);
	message += "\nExiting because this can cause problems. Increase N_KEEP_GEN_JET to avoid this error.\n";
	throw std::domain_error(message);
    }
   
    
    for (size_t i=0; i< nGenPart; i++) {
	if (std::abs(GenPart_pdgId[i]) == PID_ELECTRON || std::abs(GenPart_pdgId[i]) == PID_MUON) {
	    if (GenPart_pt[i]< 5 || abs(GenPart_pdgId[GenPart_statusFlags[i]]) != 24) continue;
	    Leptons.push_back(GenPart());
	    Leptons.back().SetupGen(GenPart_pt[i], GenPart_eta[i], GenPart_phi[i], GenPart_mass[i],
				    std::abs(GenPart_pdgId[i]));
	}
    }
    
    for (size_t j=0; j<nGenJet; j++) {
      Jets.push_back(GenJet());
      Jets.back().SetupGenJet(GenJet_pt[j], GenJet_eta[j], GenJet_phi[j], GenJet_mass[j], std::abs(GenJet_partonFlavour[j]));
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
    
    for(auto reco_jet : TTTAna.goodJets) {
      bool isJetMatched = false;
      for(auto& gen_jet : Jets) {	
    	if (reco::deltaR(gen_jet.gjVector(), reco_jet.v)<0.05) {
	  gen_jet.SetupRecoJet(reco_jet);
    	  isJetMatched = true;
	  if (reco_jet.Id() ==PID_BJET && gen_jet.gjId() == PID_BJET) {
	    gen_jet.isBJetMatched = true;
	  }
    	  break;
    	}
      }
      if(!isJetMatched) {
     	Jets.push_back(GenJet());
     	Jets.back().SetupRecoJet(reco_jet);
      }
    }
	
    weight_g = TTTAna.weight;
    
    channelName_ = "all";
    channel_ = channelMap_[channelName_];
}

void Efficiency::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) {
    int nGenElec = 0;
    int nGenMuon = 0;
    int nGenBJet = 0;
    int nGenJet = 0;
    int nMatchBJet =0;
    int nFakeBJet = 0;

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

    for (auto jet: Jets) {
      if (jet.gjId() ==PID_BJET || jet.rjId() == PID_BJET) {
	if(jet.isBJetMatched) {
	  Fill1D("MatchBJetPt", jet.gjPt());
	  nMatchBJet++;
	}
    	else if(jet.isJFaked()) {
    	  Fill1D("FakeBJetPt", jet.rjPt());
    	  nFakeBJet++;
    	}
    	Fill1D("GenBJetPt", jet.gjPt());
    	//Fill2D("BEff_b_btag", jet.gjPt(), jet.gjEta());
    	nGenBJet ++;
      }
      else {
    	if(jet.isJMatched()) {
    	  Fill1D("MatchJetPt", jet.gjPt());
    	}
    	else if(jet.isJFaked()) {
    	  Fill1D("FakeJetPt", jet.rjPt());
    	}
    	Fill1D("GenJetPt", jet.gjPt());
    	//	Fill2D("BEff_j_btag", jet.gjPt(), jet.gjEta());
    	nGenJet ++;
      }
    }

    Fill2D("GenbJetsvsJets", nGenJet, nGenBJet);
    Fill1D("NMatchBJet", nMatchBJet);
    Fill1D("NGenBJet", nGenBJet);
    Fill1D("NGenJet", nGenJet);
    Fill1D("NGenElec", nGenElec);
    Fill1D("NGenMuon", nGenMuon);
    Fill1D("NFakeBJet", nFakeBJet);
    return;
}



