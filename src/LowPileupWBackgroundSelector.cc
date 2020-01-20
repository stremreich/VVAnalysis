#include "Analysis/VVAnalysis/interface/LowPileupWBackgroundSelector.h"
#include <TStyle.h>
#include <regex>

void LowPileupWBackgroundSelector::Init(TTree *tree) {
    isNonprompt_ = true;
    LowPileupWSelector::Init(tree);
}

void LowPileupWBackgroundSelector::FillHistograms(Long64_t entry, SystPair variation) { 
    bool isMu = channel_ == mn || channel_ == mp;
    if ((isMu && *lepRelIso > 0.25 && *lepRelIso < 0.35) ||
            (isMu && *lepRelIso > 0.3 && *lepRelIso < 0.45))
        return;
    weight *= (isMu ? 0.13 : 1);
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
