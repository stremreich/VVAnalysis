#include <TLorentzVector.h>

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
};
