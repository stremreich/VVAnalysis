#include "Analysis/VVAnalysis/interface/LowPileupWBackgroundSelector.h"
#include <TStyle.h>
#include <regex>

void LowPileupWBackgroundSelector::Init(TTree *tree) {
    isNonprompt_ = true;
    LowPileupWSelector::Init(tree);
}

void LowPileupWBackgroundSelector::FillHistograms(Long64_t entry, SystPair variation) { 
    if (*lepPfRelIso > 0.25 && *lepPfRelIso < 0.35)
        return;
    weight *= 0.12;
    LowPileupWSelector::FillHistograms(entry, variation);
}

void LowPileupWBackgroundSelector::SetupNewDirectory() {
    LowPileupWSelector::SetupNewDirectory();

    for (const auto && obj : *currentHistDir_) {
        std::string name = obj->GetName();
        TNamed* named = dynamic_cast<TNamed*>(obj);
        named->SetName(name.insert(name.length()-2, "Fakes_").c_str());
    }
    
}
