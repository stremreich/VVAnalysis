#include "Analysis/VVAnalysis/interface/Efficiency.h"

#define Fill1D(NAME, VALUE_) HistFullFill(histMap1D_, NAME, variation.second, VALUE_, weight_g);

void Efficiency::Init(TTree *tree) {
  
    b.SetTree(tree);

    allChannels_ = {"mm", "ee", "em", "all", "lll"};

    hists1D_ = {"GenMuon","GenElec"};
   
    hists2D_ = {"bJetvsJets",    "Beff_b_btag", "Beff_j_btag", "Beff_b", "Beff_j"};

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
  TTTAna.SetBranchesNanoAOD();
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

  // std::cout<<TTTAna.goodParts[0].v<<std::endl;

 
  for(size_t i =0; i< nGenPart; i++) {
    if (abs(GenPart_pdgId[i]) == 11) {
      genElec.push_back(GenPart(GenPart_pt[i], GenPart_eta[i], GenPart_phi[i], GenPart_mass[i]));
      
    }
    else if (abs(GenPart_pdgId[i]) == 13) {
      genMuon.push_back(GenPart(GenPart_pt[i], GenPart_eta[i], GenPart_phi[i], GenPart_mass[i]));
    }
  }

  //  for(size_t i=0; i< goodParts.size(); i++) {
  //  if (goodParts[i].Id() == Electron) {
     
         
 
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

  for (size_t x=0; x<genElec.size(); x++) {
    Fill1D("GenElec", genElec[x].Pt());
  }
  for (size_t y=0; y<genMuon.size(); y++) {
    Fill1D("GenMuon", genMuon[y].Pt());
  }
  // TTTAna.FillHistograms(entry, variation);
  return;
}
