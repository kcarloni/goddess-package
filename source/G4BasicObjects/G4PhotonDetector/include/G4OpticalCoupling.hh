/*
 * author:      Erik Dietz-Laursonn
 * institution: Physics Institute 3A, RWTH Aachen University, Aachen, Germany
 * copyright:   Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 */


#ifndef G4OPTICALCOUPLING_H
#define G4OPTICALCOUPLING_H

#include <G4ThreeVector.hh>
#include <G4Transform3D.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>

#include <PropertyToolsManager.hh>
#include <OpticalCouplingSensitiveDetector.hh>

#include <GODDeSS_DataStorage.hh>



// class variables begin with capital letters, local variables with small letters



///  a class generating the materials, volumes, and optical properties needed for an optical coupling and allows to access them.
class G4OpticalCoupling
{
public:
	/**
	 *  Constructor to construct an optical coupling:
	 *  - sets class variables to default values
	 *  - sets the variables for the optical coupling's placement (considering the transformation of the optical coupling relative to the reference volume and the transformation of the reference volume relative to the mother volume)
	 *  - creates materials (DefineMaterials())
	 *  - constructs the volumes (ConstructScintiVolume())
	 */
	G4OpticalCoupling( G4VPhysicalVolume* base_volume,				/**< G4OpticalCoupling%'s base volume (volume that something is to be coupled to, e.g. a scintillator tile) */
			   G4VPhysicalVolume* volume_optically_coupled_to_base_volume,	/**< volume that is to be coupled to the base volume (e.g. an SiPM) */
			   G4double coupling_edgeLength,					/**< edge length of the G4OpticalCoupling */
			   G4double coupling_width,					/**< width of the G4OpticalCoupling */
			   G4VPhysicalVolume* mother_volume,				/**< G4OpticalCoupling%'s mother volume */
			   G4String coupling_name,					/**< name of the G4OpticalCoupling%'s volume (it will be extended to distinguish between different G4OpticalCoupling%s and different volumes of one G4OpticalCoupling) */
			   G4ThreeVector surface_normal_relative_to_base,		/**< surface normal of the G4OpticalCoupling%'s front relative to the base volume */
			   G4ThreeVector centre_position_relative_to_base,		/**< G4OpticalCoupling%'s centre position relative to the base volume */
			   G4bool constructSensitiveDetector,				/**< a sensitive detector is to be constructed ("true" or "false") */
			   G4bool searchOverlaps,					/**< Geant should search for overlaps when placing the physical volumes of G4OpticalCoupling%'s ("true" or "false") */
			   PropertyToolsManager * propertyTools,			/**< pointer to the PropertyToolsManager that is to be used */
			   GODDeSS_DataStorage * dataStorage				/**< pointer to the GODDeSS_DataStorage that is to be used */
			 )
	// initialising the variables (doing it with default values, "" or "0" is just to prevent errors from wrongly initialised variables), this has to be done in the order of their appearance in the hh-file:
	: SearchOverlaps(searchOverlaps)
	, ConstructSensitiveDetector(constructSensitiveDetector)
	, PropertyTools(propertyTools)
	, DataStorage(dataStorage)
	, BaseVolume_physical(base_volume)
	, CoupledVolume_physical(volume_optically_coupled_to_base_volume)
	, MotherVolume_physical(mother_volume)
	, CouplingEdgeLength(coupling_edgeLength)
	, CouplingWidth(coupling_width)
	, CouplingName(coupling_name)
	, SurfaceNormal_rel(surface_normal_relative_to_base / surface_normal_relative_to_base.mag())
	, CentrePos_rel(centre_position_relative_to_base)
	{
		// if CouplingWidth == 0, do not create a coupling volume
		if(!CouplingWidth) return;

		// set default values
		SetDefaults();

		if(BaseVolume_physical->GetMotherLogical() != CoupledVolume_physical->GetMotherLogical() || BaseVolume_physical->GetMotherLogical() != MotherVolume_physical->GetLogicalVolume()) G4cout << G4endl << "ERROR: Mother volume for coupling could not be determined!" << G4endl << G4endl;

		// calculate the transformation
		G4ThreeVector couplingTranslation = CentrePos_rel;
		G4RotationMatrix couplingRotation = G4RotationMatrix();

		// Determine rotation parameters for given surface normal.
		G4ThreeVector originalSurfaceNormal(0, 0, 1);
		G4ThreeVector rotationAxis = originalSurfaceNormal.cross(SurfaceNormal_rel);
		G4double rotationAngle = 0.;
		if(fabs(rotationAxis.mag()) > 1e-12)
		{
			rotationAngle = acos(originalSurfaceNormal * SurfaceNormal_rel / (originalSurfaceNormal.mag() * SurfaceNormal_rel.mag()));
		}
		else if(SurfaceNormal_rel == -originalSurfaceNormal)
		{
			rotationAngle = 180. * CLHEP::deg;
			rotationAxis = G4ThreeVector(1, 0, 0);
		}

		couplingRotation.rotate(rotationAngle, rotationAxis);

		// considering the transformation of the reference volume
		couplingRotation = MotherVolume_physical->GetObjectRotationValue().inverse() * BaseVolume_physical->GetObjectRotationValue() * couplingRotation;

		// WARNING-NOTE: as transform() changes the vector it is applied to, the following 4 commands are needed instead of a 1 line command
		couplingTranslation.transform(BaseVolume_physical->GetObjectRotationValue());
		couplingTranslation += BaseVolume_physical->GetObjectTranslation();
		couplingTranslation -= MotherVolume_physical->GetObjectTranslation();
		couplingTranslation.transform(MotherVolume_physical->GetObjectRotationValue().inverse());


		Transformation = G4Transform3D(couplingRotation, couplingTranslation);


		// create materials:
		DefineMaterials();   //FIXME

		// create the volumes
		ConstructVolumes();
	}

	/**
	 *  Destructor (empty)
	 */
	~G4OpticalCoupling()
	{
	}


//   #######################   //
//   #  Getter functions:  #   //
//   #######################   //
// basic name of the object:
	/** @return the G4OpticalCoupling%'s name */
	G4String GetCouplingName()
	{ return CouplingName; }

// dimensions of the object:
	/** @return the G4OpticalCoupling%'s dimensions */
	G4ThreeVector GetCouplingDimensions()
	{ return G4ThreeVector(CouplingEdgeLength, CouplingEdgeLength, CouplingWidth); }

// position and orientation of the object:
	/** @return the transformation of the G4OpticalCoupling inside the mother volume */
	G4Transform3D GetCouplingTransformation()
	{ return Transformation; }

	/** @return the surface normal of the G4OpticalCoupling%'s front relative to the base volume */
	G4ThreeVector GetCouplingRelativeSurfaceNormal()
	{ return SurfaceNormal_rel; }

	/** @return the G4OpticalCoupling%'s centre position relative to the base volume */
	G4ThreeVector GetCouplingRelativeSurfacePosition()
	{ return CentrePos_rel; }

// mother volume of the object:
	/** @return pointer to the G4OpticalCoupling%'s mother volume */
	G4VPhysicalVolume * GetMotherVolume_physicalVolume()
	{
		return MotherVolume_physical;
	}

// volumes of the object:
	/** @return pointer to the physical volume of the G4OpticalCoupling */
	G4VPhysicalVolume * GetCoupling_physicalVolume()
	{ return Coupling_physical; }

	/** @return pointer to the logical volume of the G4OpticalCoupling */
	G4LogicalVolume * GetCoupling_logicalVolume()
	{ return GetCoupling_physicalVolume()->GetLogicalVolume(); }

	/** @return pointer to the solid volume of the G4OpticalCoupling */
	G4Box * GetCoupling_solidVolume()
	{ return (G4Box *) GetCoupling_logicalVolume()->GetSolid(); }

// materials of the object:
	/** @return pointer to the material of the optical coupling */
	G4Material * GetCouplingMaterial()
	{ return Material_Coupling; }

// // optical surfaces of the object:
// 	G4OpticalSurface * GetCoatingOpticalSurface()
// 	{ return OptSurf_photonDetector; }

// sensitive detector:
	/** @return G4bool, if the G4OpticalCoupling has a sensitive detector */
	G4bool HasSensitiveDetector()
	{ return ConstructSensitiveDetector; }

	/** @return pointer to the sensitive detector */
	OpticalCouplingSensitiveDetector * GetSensitiveDetector()
	{ return ((OpticalCouplingSensitiveDetector *) G4SDManager::GetSDMpointer()->FindSensitiveDetector("CouplingSD", false)); }

// other properties of the object:
	/** @return std::vector\<G4VPhysicalVolume *\> with the G4OpticalCoupling%'s base volume as zeroth entry and the volume that is to be coupled to the base volume as first entry*/
	std::vector<G4VPhysicalVolume *> GetCoupledVolumes()
	{
		std::vector<G4VPhysicalVolume *> volumes;

		volumes.push_back(BaseVolume_physical);
		volumes.push_back(CoupledVolume_physical);

		return volumes;
	}

private:
	void DefineMaterials();
	void DefineMaterialProperties();
// 	void DefineSurfacesProperties();

	void ConstructVolumes();
	void ConstructSurface();

	void SetDefaults();



	G4bool SearchOverlaps;
	G4bool ConstructSensitiveDetector;
	PropertyToolsManager * PropertyTools;
	GODDeSS_DataStorage * DataStorage;
	G4VPhysicalVolume * BaseVolume_physical;
	G4VPhysicalVolume * CoupledVolume_physical;
	G4VPhysicalVolume * MotherVolume_physical;

// Materials & Elements
	G4Material* Material_Coupling;

// Coupling
	G4double CouplingEdgeLength;
	G4double CouplingWidth;
	G4String CouplingName;
	G4ThreeVector SurfaceNormal_rel;
	G4ThreeVector CentrePos_rel;
	G4Transform3D Transformation;
	G4VPhysicalVolume * Coupling_physical;
};

#endif
