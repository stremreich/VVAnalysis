#ifndef TTTSelector_h
#define TTTSelector_h

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

class TTTSelector : public SelectorBase {
 public :
  ScaleFactor* pileupSF_;
  ScaleFactor* muonSF_;
  ScaleFactor* eIdSF_ ;
  ScaleFactor* eGsfSF_;
  ScaleFactor* mIdSF_;
  ScaleFactor* mIsoSF_;

  // Derived variables
  bool isVBS_;
  bool passesLeptonVeto;
  Float_t weight;

  // Common variables
  Float_t genWeight;
  Float_t ZMass;
  Float_t Mass;
  Float_t MET;
  Float_t type1_pfMETPhi;

  //NanoAOD variables
  static const unsigned int N_KEEP_MU_E_ = 15;
  static const unsigned int N_KEEP_JET_ = 35;
  UInt_t nElectron;
  UInt_t nMuon;
  Float_t Electron_pt[N_KEEP_MU_E_];
  Float_t Electron_eta[N_KEEP_MU_E_];
  Float_t Electron_phi[N_KEEP_MU_E_];
  Float_t Electron_mass[N_KEEP_MU_E_];
  Float_t Muon_pt[N_KEEP_MU_E_];
  Float_t Muon_eta[N_KEEP_MU_E_];
  Float_t Muon_phi[N_KEEP_MU_E_];
  Float_t Muon_mass[N_KEEP_MU_E_];
  Int_t Electron_cutBased[N_KEEP_MU_E_];
  Int_t Electron_charge[N_KEEP_MU_E_];
  Int_t Muon_charge[N_KEEP_MU_E_];
  Bool_t Muon_tightId[N_KEEP_MU_E_];
  Bool_t Muon_mediumId[N_KEEP_MU_E_];
  UChar_t Muon_tkIsoId[N_KEEP_MU_E_];
  Float_t Muon_pfRelIso04_all[N_KEEP_MU_E_];
  Float_t Muon_miniPFRelIso_all[N_KEEP_MU_E_];

  Int_t numPU;

  UInt_t nJet;
  Float_t Jet_btagCSVV2[N_KEEP_JET_];
  Float_t Jet_btagDeepB[N_KEEP_JET_];
  Float_t Jet_eta[N_KEEP_JET_];
  Float_t Jet_phi[N_KEEP_JET_];
  Float_t Jet_pt[N_KEEP_JET_];
  Float_t Jet_mass[N_KEEP_JET_];
  

  std::vector<GoodPart> goodParts;
  
  
  // Readers to access the data (delete the ones you do not need).
  //virtual void    SetScaleFactors() override;
  virtual void    SlaveBegin(TTree *tree) override;
  virtual void    Init(TTree *tree) override;

  ClassDefOverride(TTTSelector,0);

 protected:
  std::vector<std::string> nonprompt3l_ = {
    "tt-lep", "st-schan", "st-tchan-t", "st-tchan-tbar",
    "st-tw", "st-tbarw", "DYm50", "DYm50-1j",
    "DYm50-2j","DYm50-3j","DYm50-4j", "DYm50__LO",
  };

  bool isNonpromptEstimate_;
  bool isNonpromptMC_;
  bool isZgamma_;
  const float FR_MAX_PT_ = 50;
  const float FR_MAX_ETA_ = 2.5;
  bool IsTightMuon(size_t);
  bool IsTightElectron(size_t);
  bool IsTightJet(size_t);
  bool IsTightBJet(size_t);
  bool IsOverlap(size_t);

  int getSRBin();
  double HT;
  int nTightJet, nBJets;
  
  virtual std::string GetNameFromFile() override;
    virtual void    SetBranchesNanoAOD() override;
    virtual void    SetBranchesUWVV() override;
    void LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    void LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    void ApplyScaleFactors();
    void SetShiftedMasses();
    void FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    virtual void    SetupNewDirectory() override;

    BranchManager b;
  
  };

#endif

