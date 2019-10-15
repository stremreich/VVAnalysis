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
    bool isBTagged;
    
GoodPart(double pt, double eta, double phi, double m, int pdg=0 ) : v(pt, eta, phi, m), pdgId(pdg) {}
GoodPart(LorentzVector v_, int pdg=0 ) : v(v_), pdgId(pdg) {}
    
    void SetPdgId(int pdg) {
	pdgId = pdg;
    }
    
    
    double Pt() {return v.Pt();}
    double Eta() {return v.Eta();}
    double Phi() {return v.Phi();}

    int Charge() {return pdgId;}
    int Id() {return std::abs(pdgId);}

    void setPassJetSel() {jetStatus += PASS_JET;}
    void setPassBJetSel() {jetStatus += PASS_BJET;}

    bool passedJetSel() {return jetStatus % 2 == 1;}
    bool passedBJetSel() {return jetStatus / 2 == 1;}
};

#endif
