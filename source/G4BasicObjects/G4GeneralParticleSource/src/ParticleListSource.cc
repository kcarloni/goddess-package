#include "ParticleListSource.hh"

#include <fstream>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>

#include <G4ParticleTable.hh>
#include <CLHEP/Units/SystemOfUnits.h>

ParticleListSource::ParticleListSource()
: ParticleGun(new G4ParticleGun(1))
, Messenger(new ParticleListSourceMessenger(this))
, CurrentRunIndex(0)
, CurrentEventIndex(0)
, LastCsvEventId(-1)
{
}

ParticleListSource::~ParticleListSource()
{
	delete Messenger;
	delete ParticleGun;
}

size_t ParticleListSource::GetNumberOfEvents() const
{
	size_t total = 0;
	for (const auto& run : Runs) {
		total += run.events.size();
	}
	return total;
}

void ParticleListSource::LoadFile(const G4String& csvPath)
{
	Runs.clear();
	CurrentRunIndex = 0;
	CurrentEventIndex = 0;

	std::ifstream infile(csvPath.c_str());
	if (!infile.is_open())
	{
		G4cerr << "ParticleListSource::LoadFile: cannot open \"" << csvPath << "\"" << G4endl;
		return;
	}

	// Detect number of columns from the first non-comment, non-empty line
	// to determine if run_id column is present (11 cols) or not (10 cols)
	bool hasRunId = false;
	std::streampos startPos = infile.tellg();
	std::string probeLine;
	while (std::getline(infile, probeLine))
	{
		if (probeLine.empty() || probeLine[0] == '#') continue;
		// Count commas to determine column count
		int commaCount = std::count(probeLine.begin(), probeLine.end(), ',');
		hasRunId = (commaCount >= 10); // 11 columns = 10 commas
		break;
	}
	infile.clear();
	infile.seekg(startPos);

	// Read all rows, grouped by (run_id, event_id)
	// Use ordered maps to preserve insertion order
	struct RunEventKey { int run_id; int event_id; };
	std::map<int, std::map<int, std::vector<ParticleData>>> runEventMap;
	std::vector<int> runOrder;
	std::map<int, std::vector<int>> eventOrderPerRun;

	std::string line;
	bool headerSkipped = false;
	while (std::getline(infile, line))
	{
		if (line.empty() || line[0] == '#') continue;

		if (!headerSkipped)
		{
			headerSkipped = true;
			if (!std::isdigit(line[0]) && line[0] != '-') continue;
		}

		// Replace commas with spaces
		for (char& c : line) { if (c == ',') c = ' '; }

		std::istringstream iss(line);
		int runId = 0;
		int eventId;
		ParticleData p;

		if (hasRunId)
		{
			if (!(iss >> runId >> eventId >> p.pdg >> p.t_ns >> p.x_mm >> p.y_mm >> p.z_mm
			          >> p.ekin_MeV >> p.dx >> p.dy >> p.dz))
			{
				G4cerr << "ParticleListSource::LoadFile: skipping malformed line: " << line << G4endl;
				continue;
			}
		}
		else
		{
			if (!(iss >> eventId >> p.pdg >> p.t_ns >> p.x_mm >> p.y_mm >> p.z_mm
			          >> p.ekin_MeV >> p.dx >> p.dy >> p.dz))
			{
				G4cerr << "ParticleListSource::LoadFile: skipping malformed line: " << line << G4endl;
				continue;
			}
		}

		// Track ordering
		if (runEventMap.find(runId) == runEventMap.end())
		{
			runOrder.push_back(runId);
		}
		if (runEventMap[runId].find(eventId) == runEventMap[runId].end())
		{
			eventOrderPerRun[runId].push_back(eventId);
		}
		runEventMap[runId][eventId].push_back(p);
	}

	// Build Runs vector in file order
	for (int rid : runOrder)
	{
		RunBatch batch;
		batch.run_id = rid;
		for (int eid : eventOrderPerRun[rid])
		{
			batch.event_ids.push_back(eid);
			batch.events.push_back(std::move(runEventMap[rid][eid]));
		}
		Runs.push_back(std::move(batch));
	}

	G4cout << "ParticleListSource: loaded " << GetNumberOfEvents() << " events in "
	       << Runs.size() << " run(s) from \"" << csvPath << "\"" << G4endl;
}

void ParticleListSource::GeneratePrimaries(G4Event* anEvent)
{
	if (CurrentRunIndex >= Runs.size())
	{
		G4cerr << "ParticleListSource::GeneratePrimaries: no more runs available" << G4endl;
		return;
	}

	const RunBatch& run = Runs[CurrentRunIndex];
	if (CurrentEventIndex >= run.events.size())
	{
		G4cerr << "ParticleListSource::GeneratePrimaries: no more events in run "
		       << run.run_id << " (index " << CurrentEventIndex << " >= "
		       << run.events.size() << ")" << G4endl;
		return;
	}

	LastCsvEventId = run.event_ids[CurrentEventIndex];
	if (EventIdCallback) EventIdCallback(LastCsvEventId);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	const std::vector<ParticleData>& particles = run.events[CurrentEventIndex];

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
