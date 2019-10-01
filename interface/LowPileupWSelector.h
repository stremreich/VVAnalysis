#ifndef LowPileupWSelector_h
#define LowPileupWSelector_h

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
#include "Analysis/VVAnalysis/interface/LowPileupSelector.h"

#include "Analysis/VVAnalysis/interface/helpers.h"

class LowPileupWSelector : public LowPileupSelector {
public :
    // Derived values
    TLorentzVector wCand;

    TLorentzVector* lep;

    TBranch* lep_b;

    enum { eMuMu2HLT=1, eMuMu1HLT, eMuMuNoSel, eMuSta, eMuTrk };
    
    // Readers to access the data (delete the ones you do not need).
    virtual void    Init(TTree *tree) override;
    LowPileupWSelector(TTree * /*tree*/ =0) { }
    ~LowPileupWSelector() { }

    ClassDefOverride(LowPileupWSelector,0);

protected:
    virtual void    SetBranchesBacon() override;
    void LoadBranchesBacon(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    virtual void SetComposite() override;
    void FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) override;
};

#endif




