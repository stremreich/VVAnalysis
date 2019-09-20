#ifndef _GOOD_PARTICLE_H_
#define _GOOD_PARTICLE_H_

#include "helpers.h"

// Wrapper for lepton objects so to ignore problems with
// accessing data if particle is muon vs electron
enum Match {NONE = 0, GEN_ONLY =1, RECO_ONLY =2, MATCHED =3};

struct GoodPart {
    LorentzVector v;
    int pdgId;
    bool isBTagged;
    int index;
  
    GoodPart(){}
  
    GoodPart(double pt, double eta, double phi, double m ) : v(pt, eta, phi, m) {}

  void SetLorentzV(double pt, double eta, double phi, double m ) {
    v = LorentzVector(pt, eta, phi, m);
  }

    void SetPdgId(int pdg) {
	pdgId = pdg;
    }
      
    double Pt() {return v.Pt();}
    double Eta() {return v.Eta();}
    double Phi() {return v.Phi();}
    double M() {return v.M();}

    int Charge() {return pdgId;}
    int Id() {return std::abs(pdgId);}
};

struct GenPart {
  GoodPart gen;
  GoodPart reco;
     
  int status = NONE;
  
  void SetupGen(double pt, double eta, double phi, double m, int pdg) {
    gen = GoodPart(pt, eta, phi, m);
    gen.SetPdgId(pdg);
    status += GEN_ONLY;
  }

  void SetupReco(double pt, double eta, double phi, double m, int pdg) {
    reco = GoodPart(pt, eta, phi, m);
    reco.SetPdgId(pdg);
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
  bool isFaked() {return status ==RECO_ONLY;}
  bool noMatched() {return status ==GEN_ONLY;}
  
};

#endif
