#ifndef NanoGenSelectorBase_h
#define NanoGenSelectorBase_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <exception>
#include <iostream>

// Headers needed by this particular selector
#include <vector>
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/SelectorBase.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "Analysis/VVAnalysis/interface/helpers.h"
#include "Analysis/VVAnalysis/interface/BranchManager.h"
#include "PhysicsTools/HepMCCandAlgos/interface/PDFWeightsHelper.h"

class NanoGenSelectorBase : public SelectorBase {
public :
    PDFWeightsHelper pdfweightshelper_;
    // Derived values
    reco::GenParticleCollection leptons;
    reco::GenParticleCollection neutrinos;
    std::vector<LorentzVector> jets;
    LorentzVector genMet;

    unsigned int nLeptons_ = 1;
    static const unsigned int N_KEEP_GEN_ = 300;
    static const unsigned int N_KEEP_JET_ = 30;
    static const unsigned int N_LHESCALE_WEIGHTS_ = 10;
    static const unsigned int N_LHEPDF_WEIGHTS_ = 102;
    static const unsigned int N_MC2HESSIAN_WEIGHTS_ = 60;
    float weight;
    bool doMC2H_ = false;

    TH1D* mcPdfWeights_;
    TH1D* hesPdfWeights_;
    TH1D* scaleWeights_;
    
    // Values read from file
    Float_t genWeight;
    Float_t LHEWeight;
    UInt_t nLHEScaleWeight;
    UInt_t nLHEPdfWeight;
    Float_t LHEScaleWeight[N_LHESCALE_WEIGHTS_];
    Float_t LHEPdfWeight[N_LHEPDF_WEIGHTS_];
    double LHEHessianPdfWeight[N_MC2HESSIAN_WEIGHTS_];
    UInt_t nGenPart;
    Float_t GenPart_pt[N_KEEP_GEN_];
    Float_t GenPart_eta[N_KEEP_GEN_];
    Float_t GenPart_phi[N_KEEP_GEN_];
    Float_t GenPart_mass[N_KEEP_GEN_];
    Int_t GenPart_status[N_KEEP_GEN_];
    Int_t GenPart_pdgId[N_KEEP_GEN_];
    UInt_t nGenJet;
    Float_t GenJet_pt[N_KEEP_GEN_];
    Float_t GenJet_eta[N_KEEP_GEN_];
    Float_t GenJet_phi[N_KEEP_GEN_];
    Float_t GenJet_mass[N_KEEP_GEN_];
    Float_t GenMET_pt;
    Float_t GenMET_phi;
    float ht;
    
    BranchManager b;
    
    // Readers to access the data (delete the ones you do not need).
    virtual void    Init(TTree *tree) override;
    NanoGenSelectorBase(TTree * /*tree*/ =0) { }
    ~NanoGenSelectorBase() { }
    virtual void    SetupNewDirectory() override;

    ClassDefOverride(NanoGenSelectorBase,0);

protected:
    virtual void    SetBranchesNanoAOD() override;
    virtual void    FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) override {}
    void LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    virtual void SetComposite() {}
    bool overlapsCollection(const LorentzVector& cand, reco::GenParticleCollection& collection, const float deltaRCut, size_t maxCompare);
    void buildHessian2MCSet();
};

#endif



