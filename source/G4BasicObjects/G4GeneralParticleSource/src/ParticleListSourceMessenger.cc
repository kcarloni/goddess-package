#include <ParticleListSourceMessenger.hh>
#include <ParticleListSource.hh>

ParticleListSourceMessenger::ParticleListSourceMessenger(ParticleListSource* source)
: Source(source)
{
	Directory = new G4UIdirectory("/particleList/");
	Directory->SetGuidance("Commands for the CSV particle list source.");

	FilePathCmd = new G4UIcmdWithAString("/particleList/path", this);
	FilePathCmd->SetGuidance("Set the path to the CSV file containing the particle list.");
}

ParticleListSourceMessenger::~ParticleListSourceMessenger()
{
	delete FilePathCmd;
	delete Directory;
}

void ParticleListSourceMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == FilePathCmd)
	{
		Source->LoadFile(newValue);
	}
}
