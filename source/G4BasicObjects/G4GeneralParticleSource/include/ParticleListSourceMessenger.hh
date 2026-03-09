#ifndef ParticleListSourceMessenger_h
#define ParticleListSourceMessenger_h 1

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>

class ParticleListSource;

/// UI messenger for ParticleListSource.
/// Provides the command /particleList/path to load a CSV particle list.
class ParticleListSourceMessenger : public G4UImessenger
{
public:
	ParticleListSourceMessenger(ParticleListSource* source);
	~ParticleListSourceMessenger();

	void SetNewValue(G4UIcommand* command, G4String newValue);

private:
	ParticleListSource* Source;
	G4UIdirectory* Directory;
	G4UIcmdWithAString* FilePathCmd;
};

#endif
