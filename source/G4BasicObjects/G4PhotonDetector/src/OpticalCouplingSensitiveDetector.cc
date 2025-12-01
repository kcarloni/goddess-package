/*
 * author:      Erik Dietz-Laursonn
 * institution: Physics Institute 3A, RWTH Aachen University, Aachen, Germany
 * copyright:   Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 */


// #include <G4Event.hh>
// #include <G4EventManager.hh>
#include <G4OpticalPhoton.hh>
// #include <G4PhysicalVolumeStore.hh>

#include "OpticalCouplingSensitiveDetector.hh"



// class variables begin with capital letters, local variables with small letters

G4bool OpticalCouplingSensitiveDetector::ProcessHits(G4Step* theStep, G4TouchableHistory*)
{
	G4Track* theTrack = theStep->GetTrack();
	DataStorage->FillEmptyEntriesUpToCurrentTrack(theTrack);


	G4int trackID = theTrack->GetTrackID();
	DataStorage->SetOpticalCouplingWasHit(trackID);

	/// if the particle is no optical photon, stop here
	if(theTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
	{
// 		// Stop track.
// 		theTrack->SetTrackStatus(fStopAndKill);

		return true;
	}

// 	/// if optical photon was created inside the sensitive detector, stop here
// 	G4StepPoint * thePreStepPoint = theStep->GetPreStepPoint();
// 	if(theTrack->GetLogicalVolumeAtVertex() == thePreStepPoint->GetPhysicalVolume()->GetLogicalVolume())
// 	{
// 		// Stop track.
// 		theTrack->SetTrackStatus(fStopAndKill);
//
// 		return true;
// 	}
//
//
// 	G4VPhysicalVolume * volumeThatWasHit = thePreStepPoint->GetPhysicalVolume();
// 	G4VPhysicalVolume * opticalCouplingThatWasHit = G4PhysicalVolumeStore::GetInstance()->GetVolume(volumeThatWasHit->GetName() + "_coating", false);
//
// 	DataStorage->SetNameOfVolumeThatWasHit(volumeThatWasHit->GetName());
//
// 	G4ThreeVector hitPoint = thePreStepPoint->GetPosition();
// 	hitPoint -= opticalCouplingThatWasHit->GetTranslation();
// 	hitPoint.transform(opticalCouplingThatWasHit->GetObjectRotationValue().inverse());
// 	DataStorage->SetHitPoint(hitPoint);
//
// 	hitMomentum.transform(opticalCouplingThatWasHit->GetObjectRotationValue().inverse());
// 	DataStorage->SetHitMomentum(hitMomentum);


// 	// Stop track.
// 	theTrack->SetTrackStatus(fStopAndKill);

	return true;
}
