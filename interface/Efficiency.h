#ifndef Efficiency_h
#define Efficiency_h
#include "Analysis/VVAnalysis/interface/SelectorBase.h"
#include "Analysis/VVAnalysis/interface/ThreeLepSelector.h"

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <TEfficiency.h>
#include <exception>
#include <iostream>

// Headers needed by this particular selector
#include <vector>
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/SelectorBase.h"
#include "Analysis/VVAnalysis/interface/BranchManager.h"
#include "Analysis/VVAnalysis/interface/GoodParticle.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>> LorentzVector;

class Efficiency : public SelectorBase {
 public:
  ThreeLepSelector TTTAna;
  ClassDefOverride(Efficiency, 0);

 
  //  //NanoAOD variables //keep mu e was 15 --> bumped it up to 35
  static const unsigned int N_KEEP_MU_E_ = 35;
  static const unsigned int N_KEEP_JET_ = 35;

  //// gen branches ///////////////////////////

  Float_t GenMET_pt;

  UInt_t nGenPart;
  Float_t GenPart_pt[N_KEEP_MU_E_];
  Int_t   GenPart_pdgId[N_KEEP_MU_E_];
  Float_t GenPart_eta[N_KEEP_MU_E_];
  Float_t GenPart_phi[N_KEEP_MU_E_];
  Float_t GenPart_mass[N_KEEP_MU_E_];
  

  
  

  /// reco branches ////////////////////////

  Float_t Jet_pt[N_KEEP_JET_];
  UInt_t nJet;
  Float_t MET;

  UInt_t nElectron;
  Float_t Electron_pt[N_KEEP_MU_E_];
  Int_t Electron_cutBased[N_KEEP_MU_E_];

  UInt_t nMuon;
  Float_t Muon_pt[N_KEEP_MU_E_];
  Bool_t Muon_mediumId[N_KEEP_MU_E_];
  // reco is usual stuff


  // any other functions??
  BranchManager b;
  Float_t weight_g;
  std::vector<GoodPart> goodParts;
  std::vector<GenPart> genVector;
  std::vector<GenPart> genElec;
  std::vector<GenPart> genMuon;
  std::vector<GenPart> genJet;


  void clearValues();
  
  // overloaded or necessary functions
  virtual void    SetBranchesNanoAOD() override;
  void LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) override;
  void FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) override;
  virtual void    SetupNewDirectory() override;
    virtual std::string GetNameFromFile() override {return "";}
  // Readers to access the data (delete the ones you do not need).
    virtual void    SlaveBegin(TTree *tree) override {return;}
  virtual void    Init(TTree *tree) override; //{return;}

  ///ignore
    void LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation) override {return;}
    virtual void    SetBranchesUWVV() override {return;}
};

// class ThreeLepSelector : public SelectorBase {
//  public :
//   /*****************************************/
//   /* ____  ____   ___  __  __   ___ __  __ */
//   /* || )) || \\ // \\ ||\ ||  //   ||  || */
//   /* ||=)  ||_// ||=|| ||\\|| ((    ||==|| */
//   /* ||_)) || \\ || || || \||  \\__ ||  || */
//   /*****************************************/
  
//   ScaleFactor* pileupSF_;
//   ScaleFactor* muonSF_;
//   ScaleFactor* eIdSF_ ;
//   ScaleFactor* eGsfSF_;
//   ScaleFactor* mIdSF_;
//   ScaleFactor* mIsoSF_;

//   // Common variables
//   Float_t genWeight;
//   Float_t MET;
//   Float_t type1_pfMETPhi;

//   //NanoAOD variables
//   static const unsigned int N_KEEP_MU_E_ = 15;
//   static const unsigned int N_KEEP_JET_ = 35;

//   UInt_t nElectron;
//   Float_t Electron_pt[N_KEEP_MU_E_];
//   Float_t Electron_eta[N_KEEP_MU_E_];
//   Float_t Electron_phi[N_KEEP_MU_E_];
//   Float_t Electron_mass[N_KEEP_MU_E_];
//   Int_t Electron_cutBased[N_KEEP_MU_E_];
//   Int_t Electron_charge[N_KEEP_MU_E_];
//   Float_t Electron_MVA[N_KEEP_MU_E_];
//   Float_t Electron_miniPFRelIso_all[N_KEEP_MU_E_];
//   Float_t Electron_dxy[N_KEEP_MU_E_];
//   Float_t Electron_dz[N_KEEP_MU_E_];
//   Float_t Electron_sip3d[N_KEEP_MU_E_];
//   Bool_t Electron_convVeto[N_KEEP_MU_E_];
//   UChar_t Electron_lostHits[N_KEEP_MU_E_];
  
//   UInt_t nMuon;
//   Float_t Muon_pt[N_KEEP_MU_E_];
//   Float_t Muon_eta[N_KEEP_MU_E_];
//   Float_t Muon_phi[N_KEEP_MU_E_];
//   Float_t Muon_mass[N_KEEP_MU_E_];
//   Int_t Muon_charge[N_KEEP_MU_E_];
//   Bool_t Muon_tightId[N_KEEP_MU_E_];
//   Bool_t Muon_mediumId[N_KEEP_MU_E_];
//   UChar_t Muon_tkIsoId[N_KEEP_MU_E_];
//   Float_t Muon_pfRelIso04_all[N_KEEP_MU_E_];
//   Float_t Muon_miniPFRelIso_all[N_KEEP_MU_E_];
//   Float_t Muon_dxy[N_KEEP_MU_E_];
//   Float_t Muon_dz[N_KEEP_MU_E_];
//   Float_t Muon_sip3d[N_KEEP_MU_E_];
//   Bool_t Muon_isGlobal[N_KEEP_MU_E_];
//   Bool_t Muon_isPFcand[N_KEEP_MU_E_];
//   Int_t Muon_tightCharge[N_KEEP_MU_E_];
  
//   Int_t numPU;

//   UInt_t nJet;
//   Float_t Jet_btagCSVV2[N_KEEP_JET_];
//   Float_t Jet_btagDeepB[N_KEEP_JET_];
//   Float_t Jet_eta[N_KEEP_JET_];
//   Float_t Jet_phi[N_KEEP_JET_];
//   Float_t Jet_pt[N_KEEP_JET_];
//   Float_t Jet_mass[N_KEEP_JET_];
//   Float_t Jet_neHEF[N_KEEP_JET_];
//   Float_t Jet_neEmEF[N_KEEP_JET_];
//   Int_t Jet_nConstituents[N_KEEP_JET_];
//   Float_t Jet_chHEF[N_KEEP_JET_];  
//   Float_t Jet_chEmEF[N_KEEP_JET_]; 

//   ClassDefOverride(ThreeLepSelector,0);

//   /*******************************************************/
//   /* __ __  ___  ____  __  ___  ____  __     ____  __    */
//   /* || || // \\ || \\ || // \\ || )) ||    ||    (( \   */
//   /* \\ // ||=|| ||_// || ||=|| ||=)  ||    ||==   \\    */
//   /*  \V/  || || || \\ || || || ||_)) ||__| ||___ \_))   */
//   /*******************************************************/
  
//   Float_t weight;
//   BranchManager b;
//   std::vector<GoodPart> goodParts;
//   std::vector<GoodPart> looseMuons;
//   std::vector<GoodPart> looseElectrons;
//   double HT;
//   int nTightJet, nBJets;
//   bool passZVeto;
  
//   /************************************************************/
//   /* _____ __ __ __  __   ___ ______ __   ___   __  __  __    */
//   /* ||    || || ||\ ||  //   | || | ||  // \\  ||\ || (( \   */
//   /* ||==  || || ||\\|| ((      ||   || ((   )) ||\\||  \\    */
//   /* ||    \\_// || \||  \\__   ||   ||  \\_//  || \|| \_))   */
//   /************************************************************/
  
//   /// Captial I for particle definition
//   bool isGoodMuon(size_t);
//   bool isGoodCBElectron(size_t);
//   bool isGoodJet(size_t);
//   bool isGoodBJet(size_t);
//   bool isGoodMVAElectron2016(size_t);
//   bool isGoodMVAElectron2017(size_t);
//   bool isLooseMuon(size_t);
//   bool isLooseElectron(size_t);
//   bool isLooseMVAElectron(size_t);
  
//   // lowercase I for helper function (kinda shitty convention, may change)
//   bool isLooseJetId(size_t);
//   bool isTightJetId(size_t);
//   bool doesNotOverlap(size_t);
//   bool passFullIso(LorentzVector&, int, int);
//   bool doesPassZVeto(GoodPart&, std::vector<GoodPart>&);
  
//   //// General Functions
//   int getSRBin() const;
//   void clearValues();
//   void ApplyScaleFactors();

//   // Overloaded or necesary functions
//   virtual void    SetBranchesNanoAOD() override = {}
//     void LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) override = {}
//     void FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) override = {}
//     virtual void    SetupNewDirectory() override = {}
//     virtual std::string GetNameFromFile() override = {}
//   // Readers to access the data (delete the ones you do not need).
//     virtual void    SlaveBegin(TTree *tree) override = {}
//     virtual void    Init(TTree *tree) override= {}

//   ///ignore
//     void LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation) override = {}
//     virtual void    SetBranchesUWVV() override = {}
// };

#endif

