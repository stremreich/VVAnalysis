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
  
    

    // any other functions??
    BranchManager b;
    Float_t weight_g;
    std::vector<GenPart> genVector;
    std::vector<GenPart> genLeptons;
    std::vector<GenPart> genJet;

    void clearValues();

    ClassDefOverride(Efficiency, 0);

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


#endif

