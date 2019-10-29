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
    doMC2H_ = name_.find("cp5") == std::string::npos;
    std::cout << "INFO: Convert MC to Hessian is " << doMC2H_ << std::endl;
    if (doMC2H_)
        pdfweightshelper_.Init(N_LHEPDF_WEIGHTS_, N_MC2HESSIAN_WEIGHTS_, mc2hessianCSV);
}

void NanoGenSelectorBase::SetBranchesNanoAOD() {
    b.SetBranch("genWeight", genWeight);
    b.SetBranch("nLHEScaleWeight", nLHEScaleWeight);
    b.SetBranch("nLHEPdfWeight", nLHEPdfWeight);
    b.SetBranch("LHEScaleWeight", LHEScaleWeight);
    b.SetBranch("LHEPdfWeight", LHEPdfWeight);
    b.SetBranch("nGenPart", nGenPart);
    b.SetBranch("GenPart_pt", GenPart_pt);
    b.SetBranch("GenPart_phi", GenPart_phi);
    b.SetBranch("GenPart_eta", GenPart_eta);
    b.SetBranch("GenPart_phi", GenPart_phi);
    b.SetBranch("GenPart_mass", GenPart_mass);
    b.SetBranch("GenPart_status", GenPart_status);
    b.SetBranch("GenPart_pdgId", GenPart_pdgId);
    b.SetBranch("nGenJet", nGenJet);
    b.SetBranch("GenJet_pt", GenJet_pt);
    b.SetBranch("GenJet_phi", GenJet_phi);
    b.SetBranch("GenJet_eta", GenJet_eta);
    b.SetBranch("GenJet_phi", GenJet_phi);
    b.SetBranch("GenJet_mass", GenJet_mass);
    b.SetBranch("GenMET_pt", GenMET_pt);
    b.SetBranch("GenMET_phi", GenMET_phi);
}

void NanoGenSelectorBase::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    b.SetEntry(entry);

    if (nGenPart > N_KEEP_GEN_) {
        std::string message = "Found more Gen particles than max read number.\n    Found ";
        message += std::to_string(nGenPart);
        message += " particles\n  --> Max read number was ";
        message += std::to_string(N_KEEP_GEN_);
        message += "\nExiting because this can cause problems. Increase N_KEEP_GEN_ to avoid this error.\n";
        throw std::domain_error(message);
    }

    if (nGenJet > N_KEEP_JET_) {
        std::string message = "Found more Gen jets than max read number.\n    Found ";
        message += std::to_string(nGenJet);
        message += " particles\n  --> Max read number was ";
        message += std::to_string(N_KEEP_JET_);
        message += "\nExiting because this can cause problems. Increase N_KEEP_JET_ to avoid this error.\n";
        throw std::domain_error(message);
    }

    channel_ = channelMap_[channelName_];

    leptons.clear();
    jets.clear();
    neutrinos.clear();
    std::vector<unsigned int> idsToKeep = {11, 12, 13, 14};
    for (size_t i = 0; i < nGenPart; i++) {
        if (GenPart_status[i] != 1)
            continue;
        LorentzVector vec;
        if (std::find(idsToKeep.begin(), idsToKeep.end(), std::abs(GenPart_pdgId[i])) != idsToKeep.end()) {
            vec.SetPt(GenPart_pt[i]);
            vec.SetEta(GenPart_eta[i]);
            vec.SetPhi(GenPart_phi[i]);
            vec.SetM(GenPart_mass[i]);
        }
        if (std::abs(GenPart_pdgId[i]) == 11 || std::abs(GenPart_pdgId[i]) == 13) {
            int charge = (GenPart_pdgId[i] < 0) ? -1: 1;
            leptons.push_back(reco::GenParticle(charge, vec, reco::Particle::Point(), GenPart_pdgId[i], GenPart_status[i], true));
        }
        else if (std::abs(GenPart_pdgId[i]) == 12 || std::abs(GenPart_pdgId[i]) == 14) {
            neutrinos.push_back(reco::GenParticle(0, vec, reco::Particle::Point(), GenPart_pdgId[i], GenPart_status[i], true));
        }
    }
    std::sort(leptons.begin(), leptons.end(), 
        [](const reco::GenParticle& a, const reco::GenParticle& b) { return a.pt() > b.pt(); });

    ht = 0;
    for (size_t i = 0; i < nGenJet; i++) {
        LorentzVector jet;
        jet.SetPt(GenJet_pt[i]);
        jet.SetEta(GenJet_eta[i]);
        jet.SetPhi(GenJet_phi[i]);
        jet.SetM(GenJet_mass[i]);
        if (jet.pt() > 30 && !helpers::overlapsCollection(jet, leptons, 0.4, nLeptons_)) {
            ht += jet.pt();
            jets.push_back(jet);
        }
    } // No need to sort jets, they're already pt sorted

    genMet.SetPt(GenMET_pt);
    genMet.SetPhi(GenMET_phi);
    genMet.SetM(0.);
    genMet.SetEta(0.);

    SetComposite();
    weight = genWeight;
    if (doMC2H_)
        buildHessian2MCSet();
}

void NanoGenSelectorBase::buildHessian2MCSet() {
    double pdfWeights[N_LHEPDF_WEIGHTS_];
    for (size_t i = 0; i < N_LHEPDF_WEIGHTS_; i++) {
        // Exclude alpha_s
        if (i == 100)
            break;
        pdfWeights[i] = LHEPdfWeight[i];
    }
    pdfweightshelper_.DoMC2Hessian(1., const_cast<const double *>(pdfWeights), LHEHessianPdfWeight);
}

void NanoGenSelectorBase::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();
    AddObject<TH1D>(mcPdfWeights_, "MCweights", "MC pdf weights", 200, 0, 2);
    AddObject<TH1D>(hesPdfWeights_, "Hesweights", "Hessian pdf weights", 200, 0, 2);
    AddObject<TH1D>(scaleWeights_, "scaleweights", "Scale weights", 200, 0, 2);

    InitializeHistogramsFromConfig();
}
