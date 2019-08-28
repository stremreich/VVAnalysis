#include "Analysis/VVAnalysis/interface/Efficiency.h"

#define Fill1D(NAME, VALUE_) HistFullFill(histMap1D_, NAME, variation.second, VALUE_, weight_g);

enum PID {PID_MUON = 13, PID_ELECTRON = 11, PID_BJET = 5};

void Efficiency::Init(TTree *tree) {

    b.SetTree(tree);

    allChannels_ = {"all"};
    hists1D_ = {"GenMuonPt","GenElecPt", "NGenMuon", "NGenElec"};
    //    hists2D_ = {"bJetvsJets",    "Beff_b_btag", "Beff_j_btag", "Beff_b", "Beff_j"};
    
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
    genElec.clear();
    genMuon.clear();
    genJet.clear();
}

void Efficiency::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) {
    clearValues();
    b.SetEntry(entry);
    TTTAna.LoadBranchesNanoAOD(entry,variation);

    for(size_t i =0; i< nGenPart; i++) {
	if (abs(GenPart_pdgId[i]) == 11 || abs(GenPart_pdgId[i])) {
	    genLeptons.push_back(GenPart());
	    genLeptons.back().SetupGen(GenPart_pt[i], GenPart_eta[i], GenPart_phi[i], GenPart_mass[i],
				       GenPart_pdgId[i]);
	}
	else {
	    continue;
	}
	// genLepton.back() is the just found particle
	// can put matching logic here maybe
    }

    /// I would suggest another function for the reco matching
    /// and putting that in your main 
    
    //another loop for reco
    // for(auto recoNum: goodElectrons)
    // find reco electron
    // bool found = false;
    // bool passId[idlist.size()] = {whole bunch of cuts?}
    // for(auto genVec: genVecVector) {
    // if (genVec.DeltaR(recoVec) < 0.4) {
    // Fill Histogram;
    // found= true;
    //
    //for(int i = 0; i< idlist.size(); i++) {
    // if(!found && passId[i]) {
    // HistClass::Fill("Nfake").c_str(), recovec.Pt(),1);
    channelName_ = "all";
    channel_ = channelMap_[channelName_];
}

void Efficiency::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) {

    for (auto lep: genLeptons) {
	if(lep.gId() == PID_ELECTRON) {
	    Fill1D("GenElecPt", lep.gPt());
	}
	else if(lep.gId() == PID_MUON) {
	    Fill1D("GenElecPt", lep.gPt());
	}
        
    }
    
    return;
}


