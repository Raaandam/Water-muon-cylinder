#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <map>
#include <string>

class G4Step;
class RunAction;

class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction(RunAction* runAction);
  virtual ~SteppingAction();

  virtual void UserSteppingAction(const G4Step* step) override;

private:
  RunAction* fRunAction;
};

#endif
