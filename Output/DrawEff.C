#include "TEfficiency.h" 
#include "TMath.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include <iostream>
#include <string>
#include <sstream>
#include "TStyle.h"

void DrawEff() {		

  TFile* pFile = new TFile("test.root");
  gDirectory->cd("test.root:/4top2016");

  /// interested histograms //
     
  TH1D* match_muon = (TH1D*)gDirectory->Get("MatchMuonPt_all");
  TH1D* match_elec = (TH1D*)gDirectory->Get("MatchElecPt_all");
  TH1D* match_jet = (TH1D*)gDirectory->Get("MatchJetPt_all");
  TH1D* match_bjet = (TH1D*)gDirectory->Get("MatchBJetPt_all");

  TH1D* gen_muon = (TH1D*)gDirectory->Get("GenMuonPt_all");
  TH1D* gen_elec = (TH1D*)gDirectory->Get("GenElecPt_all");
  TH1D* gen_jet = (TH1D*)gDirectory->Get("GenJetPt_all");
  TH1D* gen_bjet = (TH1D*)gDirectory->Get("GenBJetPt_all");


  std::vector<TH1D*> match_hists{};
  match_hists.push_back(match_muon);
  match_hists.push_back(match_elec);
  match_hists.push_back(match_jet);
  match_hists.push_back(match_bjet);
  
  std::vector<TH1D*> gen_hists{};
  gen_hists.push_back(gen_muon);
  gen_hists.push_back(gen_elec);
  gen_hists.push_back(gen_jet);
  gen_hists.push_back(gen_bjet);
 
  for (int i=0; i<match_hists.size(); i++) {
    TCanvas* c = new TCanvas();
    // TGraphAsymmErrors* Eff =0;
    // c->SetTitle(match_hists[i]->GetTitle());
    // Eff = new TGraphAsymmErrors(match_hists[i], gen_hists[i]);
    // std::string name = match_hists[i]->GetName();
    match_hists[i]->Divide(gen_hists[i]);
    match_hists[i] ->Draw();
    //Eff->Draw();
    c->Modified();
    c->Update();
  }
}
