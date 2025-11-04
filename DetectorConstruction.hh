#ifndef DetectorConstruction_h
#define DetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume* Construct() override;

  // helper to check FV membership from other classes
  static G4bool IsInFiducial(const G4ThreeVector& pos);

  // fiducial geometry accessors (definitions in .cc where units are available)
  static G4double FV_radius();     // returns value in internal GEANT4 units (e.g. mm)
  static G4double FV_halfHeight(); // returns value in internal GEANT4 units

private:
  G4VPhysicalVolume* fWorldPhys = nullptr;
};

#endif
