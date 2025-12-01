/*
 * author:      Erik Dietz-Laursonn
 * institution: Physics Institute 3A, RWTH Aachen University, Aachen, Germany
 * copyright:   Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 */


#include <boost/lexical_cast.hpp>

#include <OpticalCouplingConstructor.hh>
#include <OpticalCouplingPhysicsList.icc>



// class variables begin with capital letters, local variables with small letters



/**
 *  Function to construct an optical coupling between two volumes:
 *  - runs applyVolumeCounter() on \em coupling_name
 *  - creates a new G4OpticalCoupling object
 *  - resets the class variables to default values (using SetDefaults())
 *
 *  @return pointer to the created G4OpticalCoupling object
 */
G4OpticalCoupling * OpticalCouplingConstructor::CoupleVolumes( G4VPhysicalVolume* base_volume,
							       G4VPhysicalVolume* volume_optically_coupled_to_base_volume,
							       G4double coupling_edgeLength,
							       G4double coupling_width,
							       G4VPhysicalVolume* mother_volume,
							       G4String coupling_name,
							       G4ThreeVector surface_normal_relative_to_base,
							       G4ThreeVector centre_position_relative_to_base,
							       G4bool constructSensitiveDetector
								   //G4Material optical_material
								   )
{
	// if CouplingWidth == 0, do not create a coupling volume
	if(!coupling_width) return 0;

	// modify the name if another volume was named like this before
	applyVolumeCounter(coupling_name);

	// create the volumes
	G4OpticalCoupling * opticalCoupling = new G4OpticalCoupling( base_volume,
								     volume_optically_coupled_to_base_volume,
								     coupling_edgeLength,
								     coupling_width,
								     mother_volume,
								     coupling_name,
								     surface_normal_relative_to_base,
								     centre_position_relative_to_base,
								     constructSensitiveDetector,
								     SearchOverlaps,
								     PropertyTools,
								     DataStorage );

	// reset to the default values
	SetDefaults();

	// return the volumes
	return opticalCoupling;
}



/**
 *  Function for avoiding multiple volumes with the same name:
 *  - a string variable containing the name prefix has to be passed to this function
 *  - the name prefix will be extended to distinguish between different G4OpticalCoupling%s and different volumes of one G4OpticalCoupling
 *  - the extended name prefix will be passed to the original string variable
 */
void OpticalCouplingConstructor::applyVolumeCounter(G4String & physical_volume_name)
{
	if(NamePrefixVector.size())
	{
		G4int i = 1;

		// if nothing is found, stop looping
		G4bool continueWhileLoop = true;

		// loop as long as something is found
		while(continueWhileLoop)
		{
			continueWhileLoop = false;

			for(unsigned int iter = 0; iter < NamePrefixVector.size(); iter++)
			{
				// if something is found, increment the counter and continue the while loop and continue looping
				if((i == 1 && NamePrefixVector[iter] == physical_volume_name) || NamePrefixVector[iter] == (physical_volume_name + "_" + boost::lexical_cast<std::string>(i)).c_str())
				{
					i++;
					continueWhileLoop = true;
					break;
				}
			}

			// if nothing has found at last, set new physical_volume_name
			if(!continueWhileLoop && i != 1) physical_volume_name += "_" + boost::lexical_cast<std::string>(i);
		}
	}

	NamePrefixVector.push_back(physical_volume_name);
}



/**
 *  Function to set the class variables to default values:
 */
void OpticalCouplingConstructor::SetDefaults()
{
	/** - G4bool, whether a sensitive detector is to be constructed \code ConstructSensitiveDetector = false; \endcode */
	ConstructSensitiveDetector = false;

	/** - name of the G4OpticalCoupling%'s volumes (it will be extended to distinguish between different G4OpticalCoupling%s and different volumes of one G4OpticalCoupling) \code CouplingName = "opticalCoupling"; \endcode */
	CouplingName = "opticalCoupling";

	/** - surface normal of the G4OpticalCoupling%'s front (relative to the base volume) \code SurfaceNormal_rel = G4ThreeVector(0., 0., 1.); \endcode */
	SurfaceNormal_rel = G4ThreeVector(0., 0., 1.);

	/** - G4OpticalCoupling%'s centre position (relative to the base volume) \code CentrePos_rel = G4ThreeVector(0., 0., 0.); \endcode */
	CentrePos_rel = G4ThreeVector(0., 0., 0.);
}
