#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"

ActionInitialization::ActionInitialization(const std::string& spectrumFile)
 : G4VUserActionInitialization(), fSpectrumFile(spectrumFile) {}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const {
  PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(fSpectrumFile);
  SetUserAction(primary);

  RunAction* runAct = new RunAction;
  SetUserAction(runAct);

  SteppingAction* stepAct = new SteppingAction(runAct);
  SetUserAction(stepAct);
}
