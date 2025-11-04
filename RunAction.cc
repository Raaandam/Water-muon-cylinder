#include "RunAction.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include <iostream>
#include <iomanip>

RunAction::RunAction()
 : G4UserRunAction(), fNevents(0), fTotalMuonPath_cm(0.0) {}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*) {
  fNevents = 0;
  fTotalMuonPath_cm = 0.0;
  fSecondaryCounts.clear();
  fIsotopeCounts.clear();
}

void RunAction::EndOfRunAction(const G4Run* run) {
  fNevents = run->GetNumberOfEvent();
  if (fNevents == 0) {
    G4cout << "No events run." << G4endl;
    return;
  }

  // compute average path length per muon in FV
  double L_avg_cm = fTotalMuonPath_cm / static_cast<double>(fNevents); // cm
  double rho = 1.0; // g/cm^3 for water

  // For each particle compute yield:
  std::cout << "\n==== Run Summary ====\n";
  std::cout << "Events (muons) = " << fNevents << "\n";
  std::cout << "Total muon path length inside FV (cm) = " << fTotalMuonPath_cm << "\n";
  std::cout << "Average muon path length in FV per muon (cm) = " << L_avg_cm << "\n\n";

  // Secondary totals
  long totalSecondaries = 0;
  for (const auto& kv : fSecondaryCounts) totalSecondaries += kv.second;

  std::cout << "Total secondaries produced inside FV = " << totalSecondaries << "\n";
  if (L_avg_cm > 0.0) {
    double Y = (static_cast<double>(totalSecondaries) / static_cast<double>(fNevents)) / (rho * L_avg_cm); // per muon per g/cm^2
    double Y_print = Y * 1e7; // in units 10^-7 g^-1 cm^2 mu^-1
    std::cout << "Yield (all secondaries) = " << std::setprecision(6) << Y_print << "  (10^-7 g^-1 cm^2 mu^-1)\n\n";
  } else {
    std::cout << "Average path length = 0; cannot normalize.\n";
  }

  // Show top secondary particle counts
  std::cout << "\nSecondary counts by particle (top entries):\n";
  for (const auto& kv : fSecondaryCounts) {
    double Y = 0.0;
    if (L_avg_cm > 0.0) {
      Y = (static_cast<double>(kv.second) / static_cast<double>(fNevents)) / (rho * L_avg_cm) * 1e7;
    }
    std::cout << std::setw(20) << kv.first << " : " << std::setw(12) << kv.second
              << " ; Yield(1e-7) = " << std::setw(10) << std::setprecision(6) << Y << "\n";
  }

  // Isotope (ion) counts (spallation products)
  std::cout << "\nSpallation isotope / ion yields (inside FV):\n";
long totalIsotopes = 0;
for (const auto& kv : fIsotopeCounts) totalIsotopes += kv.second;
std::cout << "Total isotopes recorded = " << totalIsotopes << "\n";

for (const auto& kv : fIsotopeCounts) {
  int A = kv.first.A, Z = kv.first.Z;
  double Y = 0.0;
  if (L_avg_cm > 0.0)
    Y = (static_cast<double>(kv.second) / static_cast<double>(fNevents))
        / (rho * L_avg_cm) * 1e7;
  std::cout << "  A=" << std::setw(3) << A
            << " Z=" << std::setw(3) << Z
            << " count=" << std::setw(10) << kv.second
            << " ; Yield(1e-7)=" << std::setw(10)
            << std::setprecision(6) << Y << "\n";
}


  std::cout << "==== End of Run ====\n";
}

void RunAction::AddMuonPathLength(double len_cm) {
  fTotalMuonPath_cm += len_cm;
}

void RunAction::IncrementSecondaryCount(const std::string& pname) {
  fSecondaryCounts[pname] += 1;
}

void RunAction::IncrementIsotopeCount(const std::string&, int A, int Z) {
  IsotopeKey key{A,Z};
  fIsotopeCounts[key] += 1;
}

