#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4SystemOfUnits.hh"
#include "DetectorConstruction.hh"

SteppingAction::SteppingAction(RunAction* runAction)
 : G4UserSteppingAction(), fRunAction(runAction) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
  if (!step) return;

  const G4Track* track = step->GetTrack();

  // Only accumulate muon path length inside the fiducial when track is the primary muon (status==fAlive and parent id == 0)
  if (track->GetParentID() == 0) {
    // step length (in cm)
    double stepLen_cm = step->GetStepLength() / cm;
    G4ThreeVector mid = (step->GetPreStepPoint()->GetPosition() + step->GetPostStepPoint()->GetPosition()) * 0.5;
    if (DetectorConstruction::IsInFiducial(mid)) {
      fRunAction->AddMuonPathLength(stepLen_cm);
    }
  }

  // Count secondaries produced in this step: use GetSecondaryInCurrentStep
const auto secondaries = step->GetSecondaryInCurrentStep();
for (auto sec : secondaries) {
  if (!sec) continue;
  G4ThreeVector pos = sec->GetPosition();
  if (!DetectorConstruction::IsInFiducial(pos)) continue;

  const G4ParticleDefinition* pd = sec->GetDefinition();
  std::string pname = pd->GetParticleName();

  fRunAction->IncrementSecondaryCount(pname);

  // If this is an ion/nucleus, record its A and Z
  if (pd->GetParticleType() == "nucleus" || pd->GetParticleType() == "Ion") {
    int Z = pd->GetAtomicNumber();
    int A = pd->GetAtomicMass();
    fRunAction->IncrementIsotopeCount(pname, A, Z);
  }
}

}
