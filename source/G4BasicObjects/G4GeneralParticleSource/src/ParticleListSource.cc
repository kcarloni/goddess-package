#include "ParticleListSource.hh"

#include <fstream>
#include <sstream>
#include <cmath>
#include <map>

#include <G4ParticleTable.hh>
#include <CLHEP/Units/SystemOfUnits.h>

ParticleListSource::ParticleListSource()
: ParticleGun(new G4ParticleGun(1))
, Messenger(new ParticleListSourceMessenger(this))
, CurrentEventIndex(0)
{
}

ParticleListSource::~ParticleListSource()
{
	delete Messenger;
	delete ParticleGun;
}

void ParticleListSource::LoadFile(const G4String& csvPath)
{
	Events.clear();
	CurrentEventIndex = 0;

	std::ifstream infile(csvPath.c_str());
	if (!infile.is_open())
	{
		G4cerr << "ParticleListSource::LoadFile: cannot open \"" << csvPath << "\"" << G4endl;
		return;
	}

	// Read all rows, grouped by event_id (preserving order)
	std::map<int, std::vector<ParticleData>> eventMap;
	std::vector<int> eventOrder;

	std::string line;
	bool headerSkipped = false;
	while (std::getline(infile, line))
	{
		// Skip empty lines and comments
		if (line.empty() || line[0] == '#') continue;

		// Skip the header row
		if (!headerSkipped)
		{
			headerSkipped = true;
			// Verify it looks like a header (starts with non-digit)
			if (!std::isdigit(line[0]) && line[0] != '-') continue;
			// If it starts with a digit, it's data — don't skip
		}

		// Replace commas with spaces for easy parsing
		for (char& c : line) { if (c == ',') c = ' '; }

		std::istringstream iss(line);
		int eventId;
		ParticleData p;
		if (!(iss >> eventId >> p.pdg >> p.t_ns >> p.x_mm >> p.y_mm >> p.z_mm
		          >> p.ekin_MeV >> p.dx >> p.dy >> p.dz))
		{
			G4cerr << "ParticleListSource::LoadFile: skipping malformed line: " << line << G4endl;
			continue;
		}

		if (eventMap.find(eventId) == eventMap.end())
		{
			eventOrder.push_back(eventId);
		}
		eventMap[eventId].push_back(p);
	}

	// Build Events vector in file order
	for (int id : eventOrder)
	{
		Events.push_back(std::move(eventMap[id]));
	}

	G4cout << "ParticleListSource: loaded " << Events.size() << " events from \"" << csvPath << "\"" << G4endl;
}

void ParticleListSource::GeneratePrimaries(G4Event* anEvent)
{
	if (CurrentEventIndex >= Events.size())
	{
		G4cerr << "ParticleListSource::GeneratePrimaries: no more events available (index "
		       << CurrentEventIndex << " >= " << Events.size() << ")" << G4endl;
		return;
	}

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	const std::vector<ParticleData>& particles = Events[CurrentEventIndex];

	for (const ParticleData& p : particles)
	{
		G4ParticleDefinition* particleDef = particleTable->FindParticle(p.pdg);
		if (!particleDef)
		{
			G4cerr << "ParticleListSource: unknown PDG code " << p.pdg << ", skipping" << G4endl;
			continue;
		}

		// Normalize direction
		double mag = std::sqrt(p.dx * p.dx + p.dy * p.dy + p.dz * p.dz);
		G4ThreeVector dir(p.dx, p.dy, p.dz);
		if (mag > 0.0) dir /= mag;

		ParticleGun->SetParticleDefinition(particleDef);
		ParticleGun->SetParticleTime(p.t_ns * CLHEP::ns);
		ParticleGun->SetParticlePosition(G4ThreeVector(p.x_mm * CLHEP::mm, p.y_mm * CLHEP::mm, p.z_mm * CLHEP::mm));
		ParticleGun->SetParticleEnergy(p.ekin_MeV * CLHEP::MeV);
		ParticleGun->SetParticleMomentumDirection(dir);

		ParticleGun->GeneratePrimaryVertex(anEvent);
	}

	CurrentEventIndex++;
}
