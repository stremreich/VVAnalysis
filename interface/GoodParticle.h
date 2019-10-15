#ifndef _GOOD_PARTICLE_H_
#define _GOOD_PARTICLE_H_

#include "helpers.h"

// Wrapper for lepton objects so to ignore problems with
// accessing data if particle is muon vs electron

struct GoodPart {
    enum JET_STATUS {NONE=0, PASS_JET=1, PASS_BJET=2, PASS_BOTH=3};
    LorentzVector v;
    int pdgId;
    int jetStatus=NONE;
    int index;
    GoodPart(){}
  
    GoodPart(double pt, double eta, double phi, double m, int pdg=0 ) : v(pt, eta, phi, m), pdgId(pdg) {}
    GoodPart(LorentzVector v_, int pdg=0 ) : v(v_), pdgId(pdg) {}
    

    void SetPdgId(int pdg) {
	pdgId = pdg;
    }
      
    double Pt() {return v.Pt();}
    double Eta() {return v.Eta();}
    double Phi() {return v.Phi();}
    double M() {return v.M();}

    int Charge() {return pdgId;}
    int Id() {return std::abs(pdgId);}

    void setPassJetSel() {jetStatus += PASS_JET;}
    void setPassBJetSel() {jetStatus += PASS_BJET;}

    bool passedJetSel() {return jetStatus % 2 == 1;}
    bool passedBJetSel() {return jetStatus / 2 == 1;}
};



//Seems a little less clunky
//Can also add some help functions as time goes on
struct GenPart {
    enum Match {NONE=0, GEN_ONLY=1, RECO_ONLY=2, MATCHED=3};
    
    GoodPart gen;
    GoodPart reco;
    int status = NONE;
    
   
    void SetupGen(double pt, double eta, double phi, double m, int pdg) {
	gen = GoodPart(pt, eta, phi, m, pdg);
	status += GEN_ONLY;
    }

    void SetupReco(GoodPart& reco_) {
	reco = reco_;
	status += RECO_ONLY;
    }

    int gId() {return gen.Id();}
    double gPt() {return gen.Pt();}
    double gEta() {return gen.Eta();}
    double gPhi() {return gen.Phi();}
    double gM() {return gen.M();}
    LorentzVector gVector(){return gen.v;}

    int rId() {return reco.Id();}
    double rPt() {return reco.Pt();}
    double rEta() {return reco.Eta();}
    double rPhi() {return reco.Phi();}
    double rM() {return reco.M();}
    LorentzVector rVector(){return reco.v;}

    bool isMatched() {return status == MATCHED;}
    bool isFaked() {return status == RECO_ONLY;}
    bool noMatched() {return status ==GEN_ONLY;}
  
};

struct GenJet {
  enum Jet_Match {J_NONE=0, GEN_J_ONLY=1, RECO_J_ONLY=2, MATCHED_J=3};

  GoodPart genjet;
  GoodPart recojet;
  int j_status = J_NONE;
     
  void SetupGenJet(double pt, double eta, double phi, double m, int pdg) {
    genjet=GoodPart(pt, eta, phi, m, pdg);
    j_status +=GEN_J_ONLY;
  }
  
  // gj for gen jet
  double gjId() {return genjet.Id();}
  double gjPt() {return genjet.Pt();}
  double gjEta() {return genjet.Eta();}
  double gjPhi() {return genjet.Phi();}
  double gjM() {return genjet.M();}
  LorentzVector gjVector(){return genjet.v;}

  void SetupRecoJet(GoodPart& recojet_) {
    recojet = recojet_;
    j_status +=RECO_J_ONLY;
  }

  double rjId() {return recojet.Id();}
  double rjPt() {return recojet.Pt();}
  double rjEta() {return recojet.Eta();}
  double rjPhi() {return recojet.Phi();}
  double rjM() {return recojet.M();}
  LorentzVector rjVector(){return recojet.v;}

  bool isJMatched() {return j_status == MATCHED_J;}
  bool isJFaked() {return j_status == RECO_J_ONLY;}
  bool noJMATCHED() {return j_status == GEN_J_ONLY;}
};

#endif
