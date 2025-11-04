#include "PrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

PrimaryGeneratorAction::PrimaryGeneratorAction(const std::string& spectrumFile)
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(nullptr),
   fStartZ(25.0) // meters above center by default (start well above top of detector)
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // set muon minus as default particle (paper likely uses mu- and mu+ mix; user can change here)
  G4ParticleDefinition* mu = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  fParticleGun->SetParticleDefinition(mu);

  // Direction: vertically downgoing (0,0,-1)
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,-1.));

  // Load spectrum if provided
  if (!spectrumFile.empty()) {
    LoadSpectrum(spectrumFile);
  } else {
    // Create a fallback approximate spectrum: broken power law for sampling:
    // We'll create a coarse histogram between 6 GeV and 2000 GeV biased to get avg ~271 GeV.
    const int NBIN = 500;
    double eMin = 6.0;
    double eMax = 2000.0;
    fEbins.resize(NBIN);
    std::vector<double> pdf(NBIN, 0.0);
    for (int i=0;i<NBIN;i++){
      double x = eMin + (eMax-eMin)*(i+0.5)/NBIN;
      fEbins[i]=x;
      // example: differential spectrum ~ E^-2.7 above ~50 GeV, flatten below
      if (x < 30.0) pdf[i] = std::pow(x, -1.5);
      else pdf[i] = std::pow(x, -2.7);
    }
    // create cdf
    fCdf.resize(NBIN);
    double s=0;
    for (int i=0;i<NBIN;i++){ s += pdf[i]; fCdf[i]=s; }
    for (int i=0;i<NBIN;i++) fCdf[i] /= s;
    G4cout << "PrimaryGeneratorAction: using fallback approximate muon spectrum (no file provided)." << G4endl;
  }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete fParticleGun;
}

void PrimaryGeneratorAction::LoadSpectrum(const std::string& filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    G4cerr << "Could not open spectrum file: " << filename << " Falling back to default." << G4endl;
    return;
  }

  std::vector<double> energies;
  std::vector<double> pdf;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    if (line[0] == '#') continue;
    std::istringstream iss(line);
    double e,p;
    if (!(iss >> e >> p)) continue;
    if (e <= 0) continue;
    energies.push_back(e);
    pdf.push_back(p);
  }
  in.close();
  if (energies.empty()) {
    G4cerr << "Spectrum file empty or invalid. Falling back to default." << G4endl;
    return;
  }
  // convert pdf -> normalized cdf
  double s = 0.0;
  for (double v : pdf) s += v;
  if (s <= 0) {
    G4cerr << "Spectrum file pdf sums to zero. Falling back to default." << G4endl;
    return;
  }
  fEbins = energies;
  fCdf.resize(pdf.size());
  double cum = 0.0;
  for (size_t i=0;i<pdf.size();++i){ cum += pdf[i]/s; fCdf[i]=cum; }
  fCdf.back() = 1.0;
  G4cout << "PrimaryGeneratorAction: loaded spectrum file " << filename << " with " << fEbins.size() << " bins." << G4endl;
}

double PrimaryGeneratorAction::SampleEnergy() {
  // If a user-supplied histogram exists, sample from it
  if (!fCdf.empty()) {
    double u = G4UniformRand();
    auto it = std::lower_bound(fCdf.begin(), fCdf.end(), u);
    if (it == fCdf.end()) return fEbins.back();
    size_t idx = std::distance(fCdf.begin(), it);
    return fEbins[idx];
  }

  // Otherwise, approximate FLUKA-based Super-K muon spectrum
  // Piecewise: below 10 GeV, steep falloff; 10–100 GeV, mild; 100–1000 GeV, E^-2.7; >1000 GeV, E^-3.5
  double Emin = 6.0, Emax = 4000.0;
  double r = G4UniformRand();
  // log-uniform sampling base, then shape by weights
  double logE = std::log10(Emin) + (std::log10(Emax) - std::log10(Emin)) * r;
  double E = std::pow(10., logE);
  double w = 0.0;
  if (E < 10.)       w = std::pow(E, -1.0);
  else if (E < 100.) w = std::pow(E, -2.0);
  else if (E < 1000.)w = std::pow(E, -2.7);
  else               w = std::pow(E, -3.5);

  // Simple rejection sample to shape distribution
  double wmax = 1.0; // normalized
  double y = G4UniformRand() * wmax;
  if (y > w) return SampleEnergy(); // retry
  return E;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  // Sample energy (GeV)
  double E_GeV = SampleEnergy();
  double E_MeV = E_GeV * 1000.0;

  fParticleGun->SetParticleEnergy(E_MeV*MeV);

  // Vertex: place muon above the detector center at z = + (det halfheight + margin)
  // Detector center is at z=0 in DetectorConstruction. We choose start z sufficiently above world such that muon is "through-going"
  double startZ = fStartZ * m;
  fParticleGun->SetParticlePosition(G4ThreeVector(0.,0., startZ));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
