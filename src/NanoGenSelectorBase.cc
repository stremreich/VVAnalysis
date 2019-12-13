#include "Analysis/VVAnalysis/interface/NanoGenSelectorBase.h"
#include "PhysicsTools/HepMCCandAlgos/interface/PDFWeightsHelper.h"
#include "Analysis/VVAnalysis/interface/helpers.h"
#include <TStyle.h>
#include <regex>

void NanoGenSelectorBase::Init(TTree *tree)
{
    b.SetTree(tree);
    SelectorBase::Init(tree);
    edm::FileInPath mc2hessianCSV("PhysicsTools/HepMCCandAlgos/data/NNPDF30_lo_as_0130_hessian_60.csv");
    doMC2H_ = name_.find("cp5") == std::string::npos && false;
    std::cout << "INFO: Convert MC to Hessian is " << doMC2H_ << std::endl;
    if (doMC2H_)
        pdfweightshelper_.Init(N_LHEPDF_WEIGHTS_, N_MC2HESSIAN_WEIGHTS_, mc2hessianCSV);
    fReader.SetTree(tree);
}

void NanoGenSelectorBase::SetBranchesNanoAOD() {
}

void NanoGenSelectorBase::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    fReader.SetLocalEntry(entry);

    channel_ = channelMap_[channelName_];

    dressedLeptons.clear();
    jets.clear();

    for (size_t i = 0; i < *nGenDressedLepton; i++) {
        LorentzVector vec;
        vec.SetPt(GenDressedLepton_pt.At(i));
        vec.SetEta(GenDressedLepton_eta.At(i));
        vec.SetPhi(GenDressedLepton_phi.At(i));
        vec.SetM(GenDressedLepton_mass.At(i));
        int charge = (GenDressedLepton_pdgId.At(i) < 0) ? -1: 1;
        dressedLeptons.push_back(reco::GenParticle(charge, vec, reco::Particle::Point(), GenDressedLepton_pdgId.At(i), 1, true));
    } // No need to sort, they're already pt sorted
    
    leptons = dressedLeptons;
    if (!doBareLeptons_ && !doBornLeptons_ && !doNeutrinos_) {
        bareLeptons.clear();
        neutrinos.clear();
        std::vector<unsigned int> idsToKeep = {11, 12, 13, 14};
        for (size_t i = 0; i < *nGenPart; i++) {
            if (GenPart_status.At(i) != 1)
                continue;
            LorentzVector vec;
            if (std::find(idsToKeep.begin(), idsToKeep.end(), std::abs(GenPart_pdgId.At(i))) != idsToKeep.end()) {
                vec.SetPt(GenPart_pt.At(i));
                vec.SetEta(GenPart_eta.At(i));
                vec.SetPhi(GenPart_phi.At(i));
                vec.SetM(GenPart_mass.At(i));
            }
            if (std::abs(GenPart_pdgId.At(i)) == 11 || std::abs(GenPart_pdgId.At(i)) == 13) {
                int charge = (GenPart_pdgId.At(i) < 0) ? -1: 1;
                leptons.push_back(reco::GenParticle(charge, vec, reco::Particle::Point(), GenPart_pdgId.At(i), GenPart_status.At(i), true));
            }
            else if (std::abs(GenPart_pdgId.At(i)) == 12 || std::abs(GenPart_pdgId.At(i)) == 14) {
                neutrinos.push_back(reco::GenParticle(0, vec, reco::Particle::Point(), GenPart_pdgId.At(i), GenPart_status.At(i), true));
            }
        }
    }
    std::sort(leptons.begin(), leptons.end(), 
        [](const reco::GenParticle& a, const reco::GenParticle& b) { return a.pt() > b.pt(); });

    ht = 0;
    for (size_t i = 0; i < *nGenJet; i++) {
        LorentzVector jet;
        jet.SetPt(GenJet_pt.At(i));
        jet.SetEta(GenJet_eta.At(i));
        jet.SetPhi(GenJet_phi.At(i));
        jet.SetM(GenJet_mass.At(i));
        if (jet.pt() > 30 && !helpers::overlapsCollection(jet, leptons, 0.4, nLeptons_)) {
            ht += jet.pt();
            jets.push_back(jet);
        }
    } // No need to sort jets, they're already pt sorted

    genMet.SetPt(*MET_fiducialGenPt);
    genMet.SetPhi(*MET_fiducialGenPhi);
    genMet.SetM(0.);
    genMet.SetEta(0.);

    weight = *genWeight;
    if (doMC2H_)
        buildHessian2MCSet();

    SetComposite();
}

void NanoGenSelectorBase::buildHessian2MCSet() {
    double pdfWeights[N_LHEPDF_WEIGHTS_];
    for (size_t i = 0; i < N_LHEPDF_WEIGHTS_; i++) {
        pdfWeights[i] = LHEPdfWeight[i];
    }
    pdfweightshelper_.DoMC2Hessian(1., const_cast<const double*>(pdfWeights), LHEHessianPdfWeight);
}

void NanoGenSelectorBase::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();
    AddObject<TH1D>(mcPdfWeights_, "MCweights", "MC pdf weights", 200, 0, 2);
    AddObject<TH1D>(hesPdfWeights_, "Hesweights", "Hessian pdf weights", 200, 0, 2);
    AddObject<TH1D>(scaleWeights_, "scaleweights", "Scale weights", 200, 0, 2);

    InitializeHistogramsFromConfig();
}
