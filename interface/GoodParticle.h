#include <TLorentzVector.h>

enum class pType {Muon, Electron};

struct GoodPart {
  TLorentzVector v;
  pType ptype;
};
