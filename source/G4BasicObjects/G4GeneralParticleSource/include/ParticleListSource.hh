#ifndef PARTICLELISTSOURCE_HH_
#define PARTICLELISTSOURCE_HH_

#include <vector>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>
#include <G4String.hh>

#include <ParticleListSourceMessenger.hh>

struct ParticleData
{
	int pdg;
	double t_ns;
	double x_mm, y_mm, z_mm;
	double ekin_MeV;
	double dx, dy, dz;
};

/// Primary generator that injects arbitrary particles read from a CSV file.
/// CSV columns: event_id, pdg, t_ns, x_mm, y_mm, z_mm, ekin_MeV, dx, dy, dz
/// Multiple rows with the same event_id are injected into a single Geant4 event.
class ParticleListSource : public G4VUserPrimaryGeneratorAction
{
public:
	ParticleListSource();
	~ParticleListSource();

	virtual void GeneratePrimaries(G4Event* anEvent);

	/// Number of events loaded from the CSV.
	size_t GetNumberOfEvents() const { return Events.size(); }

	/// Access the messenger (used by RunSimulation to check event count).
	ParticleListSourceMessenger* GetMessenger() { return Messenger; }

	/// Load particles from a CSV file. Called by the messenger.
	void LoadFile(const G4String& csvPath);

private:
	G4ParticleGun* ParticleGun;
	ParticleListSourceMessenger* Messenger;
	std::vector<std::vector<ParticleData>> Events;
	size_t CurrentEventIndex;
};

#endif /* PARTICLELISTSOURCE_HH_ */
