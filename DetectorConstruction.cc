#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <cmath>

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  G4NistManager* nist = G4NistManager::Instance();

  // Materials
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* water = nist->FindOrBuildMaterial("G4_WATER");
  // Use simple rock material; you can replace with a specific composition if desired
  G4Material* rock = nist->FindOrBuildMaterial("G4_ROCK");

  // World
  G4double worldSize = 100.0 * m;
  G4Box* solidWorld = new G4Box("World", worldSize/2.0, worldSize/2.0, worldSize/2.0);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
  fWorldPhys = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, true);

  // Inner detector: cylinder of water (diameter 39.3 m, height 41.4 m)
  G4double det_diam = 39.3 * m;
  G4double det_radius = det_diam / 2.0;
  G4double det_halfheight = (41.4 * m) / 2.0;
  G4Tubs* solidWater = new G4Tubs("Water", 0.0, det_radius, det_halfheight, 0.*deg, 360.*deg);
  G4LogicalVolume* logicWater = new G4LogicalVolume(solidWater, water, "Water");
  new G4PVPlacement(nullptr, G4ThreeVector(), logicWater, "Water", logicWorld, false, 0, true);

  // 2 m rock shell outside the cylinder (approximate)
  G4double rock_thick = 2.0 * m;
  G4double rock_outer_radius = det_radius + rock_thick;
  G4double rock_halfheight = det_halfheight + rock_thick;
  G4Tubs* solidRock = new G4Tubs("Rock", det_radius, rock_outer_radius, rock_halfheight, 0.*deg, 360.*deg);
  G4LogicalVolume* logicRock = new G4LogicalVolume(solidRock, rock, "Rock");
  new G4PVPlacement(nullptr, G4ThreeVector(), logicRock, "Rock", logicWorld, false, 0, true);

  // Fiducial volume: radius 15.15 m and half-height 16.2 m
  G4double fv_r = 15.15 * m;
  G4double fv_hh = 16.2 * m;
  G4Tubs* solidFV = new G4Tubs("FV", 0.0, fv_r, fv_hh, 0.*deg, 360.*deg);
  G4LogicalVolume* logicFV = new G4LogicalVolume(solidFV, water, "FiducialVolume");
  new G4PVPlacement(nullptr, G4ThreeVector(), logicFV, "FiducialVolume", logicWater, false, 0, true);

  // Visualization attributes (optional)
  G4VisAttributes* visWater = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.1));
  visWater->SetVisibility(true);
  logicWater->SetVisAttributes(visWater);

  G4VisAttributes* visRock = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
  visRock->SetVisibility(true);
  logicRock->SetVisAttributes(visRock);

  G4VisAttributes* visFV = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.1));
  visFV->SetVisibility(true);
  logicFV->SetVisAttributes(visFV);

  return fWorldPhys;
}
dd
// Return values are in GEANT4 internal units (here we return values with units)
G4double DetectorConstruction::FV_radius() {
  return 15.15 * m;
}
G4double DetectorConstruction::FV_halfHeight() {
  return 16.2 * m;
}

G4bool DetectorConstruction::IsInFiducial(const G4ThreeVector& pos) {
  // FV centered at origin
  G4double r = std::sqrt(pos.x()*pos.x() + pos.y()*pos.y());
  if (r <= 15.15 * m && std::fabs(pos.z()) <= 16.2 * m) return true;
  return false;
}
