#ifndef ActionInitialization_h
#define ActionInitialization_h

#include "G4VUserActionInitialization.hh"
#include <string>

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(const std::string& spectrumFile = "");
  virtual ~ActionInitialization();

  virtual void Build() const;

private:
  std::string fSpectrumFile;
};

#endif
