/*
 * author:      Erik Dietz-Laursonn
 * institution: Physics Institute 3A, RWTH Aachen University, Aachen, Germany
 * copyright:   Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 */


#ifndef OPTICALCOUPLINGCONSTRUCTOR_H
#define OPTICALCOUPLINGCONSTRUCTOR_H

#include <G4VModularPhysicsList.hh>
#include <G4ThreeVector.hh>
#include <G4Transform3D.hh>
#include <G4VPhysicalVolume.hh>

#include <G4OpticalCoupling.hh>
#include <PropertyToolsManager.hh>
#include <GODDeSS_DataStorage.hh>



// class variables begin with capital letters, local variables with small letters



///  a class making it easier and more flexible to create G4OpticalCoupling objects. <b> Additionally, it automatically deals with the registration of the needed physics! </b>
class OpticalCouplingConstructor
{
public:

	/**
	 *  Constructor:
	 *  - sets class variables to default values
	 *  - adds the needed physics processes (specified in OpticalCouplingPhysicsList) to the physics list
	 */
	OpticalCouplingConstructor( G4VModularPhysicsList * userPhysicsList,	/**< physics list which is used for the simulation */
				    PropertyToolsManager * propertyTools,	/**< pointer to the PropertyToolsManager that is to be used */
				    GODDeSS_DataStorage * dataStorage,		/**< pointer to the GODDeSS_DataStorage that is to be used */
				    G4bool searchOverlaps,			/**< Geant should search for overlaps when placing the physical volumes of G4PhotonDetector%'s ("true" or "false") */
				    int verbose = 0				/**< verbosity level for adding the needed physics processes (specified in PhotonDetectorPhysicsList) to the physics list (default: 0) */
				  )
	// initialising the variables (doing it with default values, "" or "0" is just to prevent errors from wrongly initialised variables), this has to be done in the order of their appearance in the hh-file:
	: SearchOverlaps(searchOverlaps)
	, PropertyTools(propertyTools)
	, DataStorage(dataStorage)
	{
		LoadPhysicsList(userPhysicsList, verbose);

		SetDefaults();
	}

	/**
	 *  Destructor (empty)
	 */
	~OpticalCouplingConstructor()
	{
	}



	// These are overloaded functions. They are calling the main function (defined below) using default values for the parameters which are not specified.
	#include <OpticalCouplingConstructor.icc>



	// This is the main function for creating the volumes. It will be overloaded to allow setting the optional parameters via the function call or via set-functions.
	G4OpticalCoupling * CoupleVolumes( G4VPhysicalVolume* base_volume,				/**< G4OpticalCoupling%'s base volume (volume that something is to be coupled to, e.g. a scintillator tile) */
					   G4VPhysicalVolume* volume_optically_coupled_to_base_volume,	/**< volume that is to be coupled to the \em base_volume (e.g. an SiPM) */
					   G4double coupling_edgeLength,					/**< G4OpticalCoupling%'s edge length */
					   G4double coupling_width,					/**< G4OpticalCoupling%'s width */
					   G4VPhysicalVolume* mother_volume,				/**< G4OpticalCoupling%'s mother volume */
					   G4String coupling_name,					/**< name of the G4OpticalCoupling%'s volume (it will be extended to distinguish between different G4OpticalCoupling%s and different volumes of one G4OpticalCoupling) */
					   G4ThreeVector sensitive_surface_normal_relative_to_base,	/**< surface normal of the G4OpticalCoupling%'s front relative to the \em base_volume */
					   G4ThreeVector sensitive_centre_position_relative_to_base,	/**< G4OpticalCoupling%'s centre position relative to the \em base_volume */
					   G4bool constructSensitiveDetector				/**< a sensitive detector is to be constructed ("true" or "false") */
					 );



	// These are the set-functions.
	/**
	 *  Function to set the name of the G4OpticalCoupling%'s volumes (it will be extended to distinguish between different G4OpticalCoupling%s and different volumes of one G4OpticalCoupling).
	 *
	 *  <b> The value set by this function will only apply to the very next G4ScintillatorTile or G4Wrapping object that is created! </b>
	 */
	void SetCouplingName(G4String coupling_name)
	{
		CouplingName = coupling_name;
	}

	/**
	 *  Function to set the surface normal of the G4OpticalCoupling%'s front (relative to the base volume).
	 *
	 *  <b> The value set by this function will only apply to the very next G4ScintillatorTile or G4Wrapping object that is created! </b>
	 */
	void SetCouplingSurfaceNormalRelativeToBaseVolume(G4ThreeVector surface_normal_relative_to_base)
	{
		SurfaceNormal_rel = surface_normal_relative_to_base;
	}

	/**
	 *  Function to set the G4OpticalCoupling%'s centre position (relative to the base volume).
	 *
	 *  <b> The value set by this function will only apply to the very next G4ScintillatorTile or G4Wrapping object that is created! </b>
	 */
	void SetCouplingCentrePositionRelativeToBaseVolume(G4ThreeVector centre_position_relative_to_base)
	{
		CentrePos_rel = centre_position_relative_to_base;
	}

	/**
	 *  Function to make the next G4OpticalCoupling being constructed with a sensitive detector.
	 *
	 *  <b> The value set by this function will only apply to the very next G4ScintillatorTile or G4Wrapping object that is created! </b>
	 */
	void ConstructASensitiveDetector()
	{
		ConstructSensitiveDetector = true;
	}

private:
	void applyVolumeCounter( G4String & physical_volume_name /**< name which is to be used for naming the volumes */
			       );
	void SetDefaults();

	void registerPhysics(G4VModularPhysicsList * physicsList, G4VPhysicsConstructor * physicsConstructor);
	void LoadPhysicsList(G4VModularPhysicsList * physicsList, int verbose = 0);


	G4bool ConstructSensitiveDetector;
	G4bool SearchOverlaps;
	PropertyToolsManager * PropertyTools;
	GODDeSS_DataStorage * DataStorage;
	std::vector<G4String> NamePrefixVector;

	G4String CouplingName;
	G4ThreeVector SurfaceNormal_rel;
	G4ThreeVector CentrePos_rel;
};

#endif
