#ifndef _GENRECO_PARTICLE_H_
#define _GENRECO_PARTICLE_H_
#include "helpers.h"
// g: generated; r: reco
struct GenPart {
  LorentzVector v_g;
  int pdgId_g;
  int index;
  
  GenPart(double pt_g, double eta_g, double phi_g, double m_g ) : v_g(pt_g, eta_g, phi_g, m_g) {
  
  }

  void SetPdgId_g(int pdg_g) {
    pdgId_g = pdg_g;
  }

  
  double Pt() {return v_g.Pt();}
  double Eta() {return v_g.Eta();}
  double Phi() {return v_g.Phi();}

  // int Charge_g() {return pdgId;}
  // int Id() {return std::abs(pdgId);}
};

struct RecoPart {
  LorentzVector v_r;
  int pdgId_r;
  int index;
 
  
  RecoPart(double pt_r, double eta_r, double phi_r, double m_r ) : v_r(pt_r, eta_r, phi_r, m_r) {
  
  }

  void SetPdgId_r(int pdg_r) {
    pdgId_r = pdg_r;
  }

  double Pt() {return v_r.Pt();}
  double Eta() {return v_r.Eta();}
  double Phi() {return v_r.Phi();}

  // int Charge_g() {return pdgId;}
  // int Id() {return std::abs(pdgId);}
};

struct FakePart {
  LorentzVector v_f;
  int pdgId_f;
  int index;
  
  FakePart(double pt_f, double eta_f, double phi_f, double m_f ) : v_f(pt_f, eta_f, phi_f, m_f) {
  
  }

  void SetPdgId_f(int pdg_f) {
    pdgId_f = pdg_f;
  }

  double Pt() {return v_f.Pt();}
  double Eta() {return v_f.Eta();}
  double Phi() {return v_f.Phi();}

  // int Charge_g() {return pdgId;}
  // int Id() {return std::abs(pdgId);}
};

#endif
