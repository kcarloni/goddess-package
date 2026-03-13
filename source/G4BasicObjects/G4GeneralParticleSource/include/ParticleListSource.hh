#ifndef PARTICLELISTSOURCE_HH_
#define PARTICLELISTSOURCE_HH_

#include <vector>
#include <functional>
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

struct RunBatch
{
	int run_id;
	std::vector<int> event_ids;                    // CSV event_id for each event
	std::vector<std::vector<ParticleData>> events;
};

/// Primary generator that injects arbitrary particles read from a CSV file.
/// CSV columns (11-col): run_id, event_id, pdg, t_ns, x_mm, y_mm, z_mm, ekin_MeV, dx, dy, dz
/// CSV columns (10-col, legacy): event_id, pdg, t_ns, x_mm, y_mm, z_mm, ekin_MeV, dx, dy, dz
/// Multiple rows with the same event_id are injected into a single Geant4 event.
/// Multiple run_ids cause multiple Geant4 runs.
class ParticleListSource : public G4VUserPrimaryGeneratorAction
{
public:
	ParticleListSource();
	~ParticleListSource();

	virtual void GeneratePrimaries(G4Event* anEvent);

	/// Total number of events across all runs.
	size_t GetNumberOfEvents() const;

	/// Number of runs loaded from the CSV.
	size_t GetNumberOfRuns() const { return Runs.size(); }

	/// Get the run_id for a given run index.
	int GetRunId(size_t runIndex) const { return Runs[runIndex].run_id; }

	/// Get the number of events in a given run.
	size_t GetNumEventsInRun(size_t runIndex) const { return Runs[runIndex].events.size(); }

	/// Advance to the next run batch, resetting the event index.
	void AdvanceRun() { CurrentRunIndex++; CurrentEventIndex = 0; }

	/// Get the CSV event_id for the event that was just generated.
	int GetLastEventId() const { return LastCsvEventId; }

	/// Set a callback invoked with the CSV event_id at the start of each event.
	void SetEventIdCallback(std::function<void(int)> cb) { EventIdCallback = std::move(cb); }

	/// Free the event data for a completed run to reclaim memory.
	void FreeRunData(size_t runIndex) { Runs[runIndex].events.clear(); Runs[runIndex].events.shrink_to_fit(); Runs[runIndex].event_ids.clear(); Runs[runIndex].event_ids.shrink_to_fit(); }

	/// Access the messenger (used by RunSimulation to check event count).
	ParticleListSourceMessenger* GetMessenger() { return Messenger; }

	/// Load particles from a CSV file. Called by the messenger.
	void LoadFile(const G4String& csvPath);

private:
	G4ParticleGun* ParticleGun;
	ParticleListSourceMessenger* Messenger;
	std::vector<RunBatch> Runs;
	size_t CurrentRunIndex;
	size_t CurrentEventIndex;
	int LastCsvEventId;
	std::function<void(int)> EventIdCallback;
};

#endif /* PARTICLELISTSOURCE_HH_ */
