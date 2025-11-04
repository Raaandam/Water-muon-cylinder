#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleGun.hh"

#include <vector>
#include <string>

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction(const std::string& spectrumFile = "");
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:
  G4ParticleGun* fParticleGun;
  // for spectrum sampling
  std::vector<double> fEbins; // GeV
  std::vector<double> fCdf;
  double SampleEnergy(); // returns energy in GeV

  void LoadSpectrum(const std::string& filename);

  // geometry placement
  double fStartZ; // z coordinate above detector where muon starts (m)
};

#endif
