#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "RunAction.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT_HP.hh"
#include "G4PhysListFactory.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <memory>
#include <iostream>

int main(int argc, char** argv) {
  // parse args: <executable> <nEvents> [spectrum_file]
  int nEvents = 100000;
  std::string spectrumFile = "";
  if (argc >= 2) nEvents = std::stoi(argv[1]);
  if (argc >= 3) spectrumFile = argv[2];

  // Construct run manager
  auto runManager = G4RunManagerFactory::CreateRunManager();
  runManager->SetVerboseLevel(1);

  // Detector
  runManager->SetUserInitialization(new DetectorConstruction());

  // Physics - use factory to get FTFP_BERT_HP (includes high-precision neutrons)
  G4PhysListFactory factory;
  G4VModularPhysicsList* phys = nullptr;
  if (factory.IsReferencePhysList("FTFP_BERT_HP")) {
    phys = factory.GetReferencePhysList("FTFP_BERT_HP");
  } else {
    // fallback: instantiate directly (if available)
    phys = new FTFP_BERT_HP;
  }
  runManager->SetUserInitialization(phys);

  // Actions (supply spectrum file to Primary through ActionInitialization via environment/static arg)
  ActionInitialization* actions = new ActionInitialization(spectrumFile);
  runManager->SetUserInitialization(actions);

  // Initialize
  runManager->Initialize();

  // Visualization optional
  G4UIExecutive* ui = nullptr;
  if (argc == 1) { ui = new G4UIExecutive(argc, argv); }

  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();

  // Run
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Run with batch mode
  std::cout << "Starting run with " << nEvents << " events..." << std::endl;
  runManager->BeamOn(nEvents);

  // Cleanup
  delete visManager;
  delete ui;
  delete runManager;
  return 0;
}
