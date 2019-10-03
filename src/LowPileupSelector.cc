#include "Analysis/VVAnalysis/interface/LowPileupSelector.h"
#include <TStyle.h>
#include <regex>

void LowPileupSelector::Init(TTree *tree)
{
    b.SetTree(tree);
    SelectorBase::Init(tree);
}

void LowPileupSelector::SetBranchesBacon() {
    b.CleanUp();
    b.SetBranch("genVPt", genVPt);
    b.SetBranch("genVPhi", genVPhi);
    b.SetBranch("genVy", genVy);
    b.SetBranch("genVMass", genVMass);
    b.SetBranch("met", pfMet);
    b.SetBranch("met", pfMet);
    b.SetBranch("metPhi", pfMetPhi);
    b.SetBranch("scale1fb", scale1fb);
}

void LowPileupSelector::LoadBranchesBacon(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    b.SetEntry(entry);

    if (isMC_) {
        weight = scale1fb;
        //weight = genWeight*PUWeight*scale1fb;
    }
    SetComposite();
}

void LowPileupSelector::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}


