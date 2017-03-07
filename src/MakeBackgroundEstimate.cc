#define MakeBackgroundEstimate_cxx
// The class definition in MakeBackgroundEstimate.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following se                                              MakeBackgroundEstimate.C")
// root> T->Process("MakeBackgroundEstimate.C","some options")
// root> T->Process("MakeBackgroundEstimate.C+")
//


#include "Analysis/WZAnalysis/interface/MakeBackgroundEstimate.h"
#include <TStyle.h>

void MakeBackgroundEstimate::Begin(TTree * /*tree*/)
{
    // The Begin() function is called at the start of the query.
    // When running with PROOF Begin() is only called on the client.
    // The tree argument is deprecated (on PROOF 0 is passed).

    TString option = GetOption();
}

void MakeBackgroundEstimate::SlaveBegin(TTree * /*tree*/)
{
    // The SlaveBegin() function is called after the Begin() function.
    // When running with PROOF SlaveBegin() is called on each slave server.
    // The tree argument is deprecated (on PROOF 0 is passed).
    fakeRate_allE = (ScaleFactor *) GetInputList()->FindObject("fakeRate_allE");
    if (fakeRate_allE == nullptr ) Abort("Must pass electron fake rate to input list!");
    fakeRate_allMu = (ScaleFactor *) GetInputList()->FindObject("fakeRate_allMu");
    if (fakeRate_allMu == nullptr ) Abort("Must pass muon fake rate to input list!");

    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        TNamed* chan = (TNamed *) GetInputList()->FindObject("channel");
        if (name != nullptr) {
            name_ = name->GetTitle();
        }
        if (chan != nullptr) {
            channel_ = chan->GetTitle();
        }
    }
    histDir_ = new TList();
    histDir_->SetName(name_.c_str());
    fOutput->Add(histDir_);
    zmassHist_= new TH1D(("Zmass_Fakes_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHistPPF_= new TH1D(("Zmass_PPF_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHistPFP_= new TH1D(("Zmass_PFP_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHistFPP_= new TH1D(("Zmass_FPP_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHistFFP_= new TH1D(("Zmass_FFP_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHistFPF_= new TH1D(("Zmass_FPF_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHistPFF_= new TH1D(("Zmass_PFF_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHistFFF_= new TH1D(("Zmass_FFF_"+channel_).c_str(),
        "Zmass; M_{ll} [GeV]; Events;", 15, 76, 106);
    zmassHist_->SetDirectory(0);
    zmassHistFPP_->SetDirectory(0);
    zmassHistPFP_->SetDirectory(0);
    zmassHistPPF_->SetDirectory(0);
    zmassHistFFF_->SetDirectory(0);
    zmassHistFFP_->SetDirectory(0);
    zmassHistFPF_->SetDirectory(0);
    zmassHistPFF_->SetDirectory(0);
    histDir_->Add(zmassHist_);
    histDir_->Add(zmassHistFPP_);
    histDir_->Add(zmassHistPFP_);
    histDir_->Add(zmassHistPPF_);
    histDir_->Add(zmassHistFFF_);
    histDir_->Add(zmassHistFPF_);
    histDir_->Add(zmassHistFFP_);
    histDir_->Add(zmassHistPFF_);
    TString option = GetOption();
}

Bool_t MakeBackgroundEstimate::Process(Long64_t entry)
{
    // The Process() function is called for each entry in the tree (or possibly
    // keyed object in the case of PROOF) to be processed. The entry argument
    // specifies which entry in the currently loaded tree is to be processed.
    // When processing keyed objects with PROOF, the object is already loaded
    // and is available via the fObject pointer.
    //
    // This function should contain the \"body\" of the analysis. It can contain
    // simple or elaborate selection criteria, run algorithms on the data
    // of the event and typically fill histograms.
    //
    // The processing can be stopped by calling Abort().
    //
    // Use fStatus to set the return value of TTree::Process().
    //
    // The return value is currently not used.

    if (name_.find("data") == std::string::npos)
        b_genWeight->GetEntry(entry);
    else 
        genWeight = 1;
    b_Zmass->GetEntry(entry);
    b_Mass->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry);
    b_nWWLooseElec->GetEntry(entry);
    b_nWZLooseMuon->GetEntry(entry);
    b_l1IsTight->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l1Pt->GetEntry(entry);
    b_l2Eta->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Eta->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    if (channel_ == "eee") {
        b_e1IsEB->GetEntry(entry);
        b_e2IsEB->GetEntry(entry);
        b_e3IsEB->GetEntry(entry);
        b_e1PVDXY->GetEntry(entry);
        b_e2PVDXY->GetEntry(entry);
        b_e3PVDXY->GetEntry(entry);
        b_e1PVDZ->GetEntry(entry);
        b_e2PVDZ->GetEntry(entry);
        b_e3PVDZ->GetEntry(entry);
    }
    else if (channel_ == "eem") {
        b_e1IsEB->GetEntry(entry);
        b_e2IsEB->GetEntry(entry);
        b_e1PVDXY->GetEntry(entry);
        b_e2PVDXY->GetEntry(entry);
        b_e1PVDZ->GetEntry(entry);
        b_e2PVDZ->GetEntry(entry);
        b_m3RelPFIsoDBR04->GetEntry(entry);
    }
    else if (channel_ == "emm") {
        b_e3IsEB->GetEntry(entry);
        b_e3PVDXY->GetEntry(entry);
        b_e3PVDZ->GetEntry(entry);
        b_m1RelPFIsoDBR04->GetEntry(entry);
        b_m2RelPFIsoDBR04->GetEntry(entry);
    }
    else if (channel_ == "mmm") {
        b_m1RelPFIsoDBR04->GetEntry(entry);
        b_m2RelPFIsoDBR04->GetEntry(entry);
        b_m3RelPFIsoDBR04->GetEntry(entry);
    }
    if (nWWLooseElec + nWZLooseMuon > 3)
        return true;
    //if (type1_pfMETEt < 30)
    //    return true;
    //if (Mass < 100)
    //    return true;
    //if (Zmass > 116.1876 || Zmass < 76.1876)
    //    return true;
    if (IsFPPRegion()) {
        zmassHistFPP_->Fill(Zmass, genWeight);
        zmassHist_->Fill(Zmass, getl1FakeRate()*genWeight);
    }
    else if (IsPFPRegion()) {
        zmassHistPFP_->Fill(Zmass, genWeight);
        zmassHist_->Fill(Zmass, getl2FakeRate()*genWeight);
    }
    else if (IsPPFRegion()) {
        zmassHistPPF_->Fill(Zmass, genWeight);
        zmassHist_->Fill(Zmass, getl3FakeRate()*genWeight);
    }
    else if (IsFFFRegion()) {
        zmassHistFFF_->Fill(Zmass, genWeight);
        zmassHist_->Fill(Zmass, getl1FakeRate()*getl2FakeRate()*getl3FakeRate()*genWeight);
    }
    else if (IsFPFRegion()) {
        zmassHistFPF_->Fill(Zmass, genWeight);
        zmassHist_->Fill(Zmass, -1*getl1FakeRate()*getl3FakeRate()*genWeight);
    }
    else if (IsFFPRegion()) {
        zmassHistFFP_->Fill(Zmass, genWeight);
        zmassHist_->Fill(Zmass, -1*getl1FakeRate()*getl2FakeRate()*genWeight);
    }
    else if (IsPFFRegion()) {
        zmassHistPFF_->Fill(Zmass, genWeight);
        zmassHist_->Fill(Zmass, -1*getl2FakeRate()*getl3FakeRate()*genWeight);
    }
    return kTRUE;
}

float MakeBackgroundEstimate::getl1FakeRate() {
    float pt_eval = l1Pt < 100 ? l1Pt : 99;
    float fr = 1;
    if (channel_ == "eem" || channel_ == "eee")
        fr = fakeRate_allE->Evaluate2D(pt_eval, std::abs(l1Eta));
    else
        fr = fakeRate_allMu->Evaluate2D(pt_eval, std::abs(l1Eta));
    return fr/(1-fr); 
}

float MakeBackgroundEstimate::getl2FakeRate() {
    float pt_eval = l2Pt < 100 ? l2Pt : 99;
    float fr = 1;
    if (channel_ == "eem" || channel_ == "eee")
        fr = fakeRate_allE->Evaluate2D(pt_eval, std::abs(l2Eta));
    else
        fr = fakeRate_allMu->Evaluate2D(pt_eval, std::abs(l2Eta));
    return fr/(1-fr); 
}

float MakeBackgroundEstimate::getl3FakeRate() {
    float pt_eval = l3Pt < 100 ? l3Pt : 99;
    float fr = 1;
    if (channel_ == "emm" || channel_ == "eee")
        fr = fakeRate_allE->Evaluate2D(pt_eval, std::abs(l3Eta));
    else
        fr = fakeRate_allMu->Evaluate2D(pt_eval, std::abs(l3Eta));
    return fr/(1-fr); 
}

bool MakeBackgroundEstimate::IsFPPRegion() {
    return !zlep1IsTight() && zlep2IsTight() && wlepIsTight();
}
bool MakeBackgroundEstimate::IsPFPRegion() {
    return zlep1IsTight() && !zlep2IsTight() && wlepIsTight();
}

bool MakeBackgroundEstimate::IsPPFRegion() {
    return zlep1IsTight() && zlep2IsTight() && !wlepIsTight();
}

bool MakeBackgroundEstimate::IsFPFRegion() {
    return !zlep1IsTight() && zlep2IsTight() && !wlepIsTight();
}

bool MakeBackgroundEstimate::IsFFPRegion() {
    return !zlep1IsTight() && !zlep2IsTight() && wlepIsTight();
}

bool MakeBackgroundEstimate::IsPFFRegion() {
    return zlep1IsTight() && !zlep2IsTight() && !wlepIsTight();
}

bool MakeBackgroundEstimate::IsFFFRegion() {
    return !zlep1IsTight() && !zlep2IsTight() && !wlepIsTight();
}

bool MakeBackgroundEstimate::zlep1IsTight() {
    if (channel_ == "eem" || channel_ == "eee") {
        return (l1IsTight &&
            (e1IsEB ? e1PVDXY < 0.05 : e1PVDXY < 0.1) &&
            (e1IsEB ? e1PVDZ < 0.1 : e1PVDZ < 0.2));
    }
    else 
        return l1IsTight && (m1RelPFIsoDBR04 < 0.15);
}

bool MakeBackgroundEstimate::zlep2IsTight() {
    if (channel_ == "eem" || channel_ == "eee") {
        return (l2IsTight &&
            (e2IsEB ? e2PVDXY < 0.05 : e2PVDXY < 0.1) &&
            (e2IsEB ? e2PVDZ < 0.1 : e2PVDZ < 0.2));
    }
    else 
        return l2IsTight && (m2RelPFIsoDBR04 < 0.15);
}

bool MakeBackgroundEstimate::wlepIsTight() {
    if (channel_ == "emm" || channel_ == "eee") {
        return (l3IsTight &&
            (e3IsEB ? e3PVDXY < 0.05 : e3PVDXY < 0.1) &&
            (e3IsEB ? e3PVDZ < 0.1 : e3PVDZ < 0.2));
    }
    else 
        return l3IsTight && (m3RelPFIsoDBR04 < 0.15);
}

void MakeBackgroundEstimate::SlaveTerminate()
{
    // The SlaveTerminate() function is called after all entries or objects
    // have been processed. When running with PROOF SlaveTerminate() is called
    // on each slave server.

}

void MakeBackgroundEstimate::Terminate()
{
    // The Terminate() function is the last function to be called during
    // a query. It always runs on the client, it can be used to present
    // the results graphically or save the results to file.
}
