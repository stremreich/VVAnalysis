#ifndef _GENRECO_PARTICLE_H_
#define _GENRECO_PARTICLE_H_
#include "helpers.h"
// g: generated; r: reco

/* struct GoodPart { */
/*     LorentzVector v; */
/*     int pdgId; */
/*     bool isBTagged; */
/*     int index; */
    
/*     GoodPart(double pt, double eta, double phi, double m ) : v(pt, eta, phi, m) {} */

/*     void SetPdgId(int pdg) { */
/* 	pdgId = pdg; */
/*     } */
      
/*     double Pt() {return v.Pt();} */
/*     double Eta() {return v.Eta();} */
/*     double Phi() {return v.Phi();} */

/*     int Charge() {return pdgId;} */
/*     int Id() {return std::abs(pdgId);} */
/* }; */


/* struct GenPart { */
/*   LorentzVector v; */
/*   int pdgId; */
/*   int index; */
  
/*   GenPart(double pt, double eta, double phi, double m ) : v(pt, eta, phi, m) { */
  
/*   } */

/*   void SetPdgId(int pdg) { */
/*     pdgId = pdg; */
/*   } */
  
/*   int Id() {return std::abs(pdgId);} */

  
/*   double Pt() {return v.Pt();} */
/*   double Eta() {return v.Eta();} */
/*   double Phi() {return v.Phi();} */

/* }; */

struct RecoPart {
  LorentzVector v;
  int pdgId;
  int index;
 
  
  RecoPart(double pt, double eta, double phi, double m ) : v(pt, eta, phi, m) {
  
  }
  void SetPdgId(int pdg) {
    pdgId = pdg;
  }
  
  int Id() {return std::abs(pdgId);}

  double Pt() {return v.Pt();}
  double Eta() {return v.Eta();}
  double Phi() {return v.Phi();}

};

struct FakePart {
  LorentzVector v;
  int pdgId;
  int index;
  
  FakePart(double pt, double eta, double phi, double m ) : v(pt, eta, phi, m) {
  
  }
  void SetPdgId(int pdg) {
    pdgId = pdg;
  }

  int Id() {return std::abs(pdgId);}
  double Pt() {return v.Pt();}
  double Eta() {return v.Eta();}
  double Phi() {return v.Phi();}

};

#endif
