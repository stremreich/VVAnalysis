#ifndef LowPileupZSelector_h
#define LowPileupZSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <TLorentzVector.h>
#include <exception>
#include <iostream>

// Headers needed by this particular selector
#include <vector>
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/SelectorBase.h"
#include "Analysis/VVAnalysis/interface/BranchManager.h"

#include "Analysis/VVAnalysis/interface/helpers.h"

class LowPileupZSelector : public SelectorBase {
public :
    // Derived values
    float weight;
    TLorentzVector zCand;

    TLorentzVector* lep1;
    TLorentzVector* lep2;
    Float_t met;
    Float_t genVPt;
    Float_t genVPhi;
    Float_t genVy;
    Float_t genVMass;
    Float_t genWeight;
    Float_t PUWeight;

    BranchManager b;
    
    // Readers to access the data (delete the ones you do not need).
    virtual void    Init(TTree *tree) override;
    LowPileupZSelector(TTree * /*tree*/ =0) { }
    ~LowPileupZSelector() { }
    virtual void    SetupNewDirectory() override;

    ClassDefOverride(LowPileupZSelector,0);

protected:
    virtual void    SetBranchesBacon() override;
    void LoadBranchesBacon(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    void SetComposite();
    void FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) override;
};

#endif



