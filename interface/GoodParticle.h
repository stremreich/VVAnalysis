#ifndef _GOOD_PARTICLE_H_
#define _GOOD_PARTICLE_H_

#include "helpers.h"
// Wrapper for lepton objects so to ignore problems with
// accessing data if particle is muon vs electron

struct GoodPart {
    LorentzVector v;
    int pdgId;
    bool isBTagged;
    int index;
    
    GoodPart(double pt, double eta, double phi, double m ) : v(pt, eta, phi, m) {}

    void SetPdgId(int pdg) {
	pdgId = pdg;
    }
    
    
    double Pt() {return v.Pt();}
    double Eta() {return v.Eta();}
    double Phi() {return v.Phi();}

    int Charge() {return pdgId;}
    int Id() {return std::abs(pdgId);}
};

// g for generated, used in Efficiency
struct GenPart {
  LorentzVector v_g, v_r;
  int pdgId_g, pdgId_r;
  
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

#endif
