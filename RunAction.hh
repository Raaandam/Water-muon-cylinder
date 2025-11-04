#ifndef RunAction_h
#define RunAction_h

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <map>
#include <string>

class RunAction : public G4UserRunAction {
public:
  RunAction();
  virtual ~RunAction();

  virtual void BeginOfRunAction(const G4Run*) override;
  virtual void EndOfRunAction(const G4Run*) override;

  // called by SteppingAction
  void AddMuonPathLength(double len_cm); // sum of muon path length inside FV in cm
  void IncrementSecondaryCount(const std::string& pname);
  void IncrementIsotopeCount(const std::string& iname, int A, int Z);


private:
  long fNevents;
  double fTotalMuonPath_cm;
  std::map<std::string, long> fSecondaryCounts;
  struct IsotopeKey {
    int A; int Z;
    bool operator<(const IsotopeKey& other) const {
      return (A<other.A) || (A==other.A && Z<other.Z);
    }
  };
  std::map<IsotopeKey,long> fIsotopeCounts;


#endif
