#include <TLorentzVector.h>

// Wrapper for lepton objects so to ignore problems with
// accessing data if particle is muon vs electron

enum class pType {Muon, Electron};

struct GoodPart {
  TLorentzVector v;
  pType ptype;
  int charge;
  
  void SetTVector(double pt, double eta, double phi, double m ) {
    v.SetPtEtaPhiM(pt, eta, phi, m);
  }

  void SetpType(pType type) {
    ptype = type;
  }

  void SetCharge(int c) {
    charge = c;
  }

  double Pt() {return v.Pt();}
  double Eta() {return v.Eta();}
  double Phi() {return v.Phi();}
};
