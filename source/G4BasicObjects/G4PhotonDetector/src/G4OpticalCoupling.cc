/*
 * author:      Erik Dietz-Laursonn
 * institution: Physics Institute 3A, RWTH Aachen University, Aachen, Germany
 * copyright:   Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 */


#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4OpticalSurface.hh>
#include <G4NistManager.hh>
#include <G4OpticalCoupling.hh>

using namespace CLHEP;   //for mathematics (e.g. CLHEP::sqrt, CLHEP::pi, CLHEP::c_light, CLHEP::h_Planck,...)



// class variables begin with capital letters, local variables with small letters



/**
 *  Function to construct all volumes and surfaces for optical couplings.
 */
void G4OpticalCoupling::ConstructVolumes()
{
// solids (dimensions):
	G4String coupling_solid_name = CouplingName + "_solid";
	G4VSolid * coupling_solid = new G4Box(coupling_solid_name, CouplingEdgeLength / 2., CouplingEdgeLength / 2., CouplingWidth / 2.);

// logical volumes (material):
	G4String coupling_logical_name = CouplingName + "_logical";
	G4LogicalVolume * coupling_logical = new G4LogicalVolume(coupling_solid, Material_Coupling, coupling_logical_name, 0, 0, 0);
// 	G4LogicalVolume * coupling_logical = new G4LogicalVolume(coupling_solid, G4Material::GetMaterial("ScintillatorMaterial"), coupling_logical_name, 0, 0, 0);

	G4VisAttributes couplingVisAtt(G4Colour::Black());
	couplingVisAtt.SetForceWireframe(true);
	couplingVisAtt.SetLineWidth(3.);
	coupling_logical->SetVisAttributes(couplingVisAtt);


	if(ConstructSensitiveDetector)
	{
		// try to find an already existing sensitive detector
		OpticalCouplingSensitiveDetector * sensitiveDetector = (OpticalCouplingSensitiveDetector *) G4SDManager::GetSDMpointer()->FindSensitiveDetector("CouplingSD", false);
		if(!sensitiveDetector)
		{
			// create a new sensitive detector
			sensitiveDetector = new OpticalCouplingSensitiveDetector("CouplingSD", DataStorage);
			// register it to Geant4
			G4SDManager::GetSDMpointer()->AddNewDetector(sensitiveDetector);
		}

		// assign it to detector parts
		coupling_logical->SetSensitiveDetector(sensitiveDetector);
	}

// physical volumes (placement):
//NOTE: G4PVPlacement puts the volume to be placed into EVERY physical volume emanating from the same logical volume (no matter whether the logical or physical volume is specified as mother volume)!
//      => If G4PVPlacement is to be able to distinguish physical volumes, for each physical volume a separate logical volume has to be created.
//      => rule of thumb: For each volume that might become a mother volume, a separate logical volume should to be created.
	Coupling_physical = new G4PVPlacement(Transformation, CouplingName, coupling_logical, MotherVolume_physical, false, 0, SearchOverlaps);

// surfaces:
//NOTE: A surface has to be defined for the borders between every two volumes.
//      It is needed to simulate optical boundary processes.
//      Exclusively in case of a perfectly smooth surface between two dielectic materials
//      (and only refractive indices needed to describe), no surface has to be defined.
//
//      In order to define different surface properties for different borders of the same volumes,
//      one has to define adjacent volumes in the simulation that butt up with the various sides
//      and are made of the same physical material as the mother.

	// getting the surface properties of the base volume
	G4LogicalBorderSurface * OptBorderSurf_BaseMother = G4LogicalBorderSurface::GetSurface(BaseVolume_physical, MotherVolume_physical);

	if(OptBorderSurf_BaseMother)
	{
		G4OpticalSurface * OptSurf_BaseCoupling = new G4OpticalSurface( * ((G4OpticalSurface*) OptBorderSurf_BaseMother->GetSurfaceProperty()) );
		new G4LogicalBorderSurface("Scintillator/Coupling", BaseVolume_physical, CoupledVolume_physical, OptSurf_BaseCoupling);
	}


	G4LogicalBorderSurface * OptBorderSurf_MotherBase = G4LogicalBorderSurface::GetSurface(MotherVolume_physical, BaseVolume_physical);

	if(OptBorderSurf_MotherBase)
	{
		G4OpticalSurface * OptSurf_CouplingBase = new G4OpticalSurface( * ((G4OpticalSurface*) OptBorderSurf_MotherBase->GetSurfaceProperty()) );
		new G4LogicalBorderSurface("Coupling/Scintillator", CoupledVolume_physical, BaseVolume_physical, OptSurf_CouplingBase);
	}
}



/**
 *  Function to define materials (compounds, alloys) and their properties:
 *  - creates new materials according to the specifications from the property file(s)
 *  - if the same materials already exist, the newly created ones are deleted
 */
void G4OpticalCoupling::DefineMaterials()   ///FIXME
{
	// NOTE The materials which have already been define (e.g. in the DetectorConstruction) are used here

	// G4Material(const G4String &name, G4double z, G4double a, G4double density, G4State state=kStateUndefined, G4double temp=STP_Temperature, G4double pressure=STP_Pressure)
	//Material_Coupling = new G4Material("tempName", 1., 1.01 * g/mole, universe_mean_density, kStateGas, 0.1 * kelvin, 1.e-19 * pascal);		// definition from Geant
	//EJ-500
	//https://eljentechnology.com/products/accessories/ej-500
	Material_Coupling = new G4Material("opticalcement_coupling", 1.2 * g/cm3, 3, kStateSolid, 223.15 * kelvin);
	Material_Coupling->AddElement(G4Element::GetElement("Silicon"), 15);
	Material_Coupling->AddElement(G4Element::GetElement("Hydrogen"), 16);
	Material_Coupling->AddElement(G4Element::GetElement("Oxygen"), 2);
	//G4cout << "Test "<<*(G4Material::GetMaterialTable()) << G4endl;

	DefineMaterialProperties();

	PropertyTools->checkIfMaterialAlreadyExists("Coupling", Material_Coupling);
	/*
	//Epoxy
	Material_Coupling = new G4Material("Coupling", 1.2 * CLHEP::g / CLHEP::cm3, 3, kStateSolid, 223.15 * kelvin);
	Material_Coupling->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"), 15);
	Material_Coupling->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 16);
	Material_Coupling->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 2);
	const double rIndex = 1.57;

	//Air South Pole
	Material_Coupling = new G4Material("Coupling", 1.064 * kg/m3, 2, kStateGas, 223.15 * kelvin, 68181.0 * pascal);
	Material_Coupling->AddElement(G4Element::GetElement("Nitrogen"), 70 * perCent);
	Material_Coupling->AddElement(G4Element::GetElement("Oxygen"), 30 * perCent);
	const double rIndex = 1. + 172. * 1e-6;
	*/
}



//NOTE: possible Properties:
//         "RINDEX":			(spectrum (in dependence of the photon energy))		(obligatory property!)
//		defines the refraction index of the material, used for boundary processes, Cerenkov radiation and Rayleigh scattering
//         "ABSLENGTH":			(spectrum (in dependence of the photon energy))
//		defines the absorption length (absorption spectrum) of the material, used for the "normal" absorption of optical photons (default is infinity, i.e. no absorption)
//		(the absorption length for the WLS process of WLS materials is specified by "WLSABSLENGTH", "ABSLENGTH" can be specified additionally to simulate a non-WLS absorption fraction)
//         "RAYLEIGH":			(spectrum (in dependence of the photon energy))
//		defines the absorption length of the material, used for the rayleigh scattering of optical photons (default is infinity, i.e. no scattering)
//
//         "SCINTILLATIONYIELD":	(constant value (energy independent))			(obligatory property for scintillator materials!)
//		defines the mean number of photons, emitted per MeV energy deposition in the scintillator material (the real number is Poisson/Gauss distributed)
//		(can also be specified separately for different particles by putting "ELECTRON...", "PROTON...", "DEUTERON...", "TRITON...", "ALPHA...", "ION..." infront of "SCINTILLATIONYIELD")
//		(default is 0, i.e. no scintillation process)
//         "RESOLUTIONSCALE":		(constant value (energy independent))
//		defines the intrinsic resolution of the scintillator material, used for the statistical distribution of the number of generated photons in the scintillation process
//		(values > 1 result in a wider distribution, values < 1 result in a narrower distribution -> 1 is to be chosen as default)
//		(default is 0)
//         "FASTCOMPONENT":		(spectrum (in dependence of the photon energy))		(at least one "...COMPONENT" is obligatory for scintillator materials!)
//		defines the emission spectrum of the material, used for the fast scintillation process	NOTE: emission spectra are NOT linearly extrapolated between two given points!
//         "SLOWCOMPONENT":		(spectrum (in dependence of the photon energy))		(at least one "...COMPONENT" is obligatory for scintillator materials!)
//		defines the emission spectrum of the material, used for the slow scintillation process	NOTE: emission spectra are NOT linearly extrapolated between two given points!
//         "FASTTIMECONSTANT":		(constant value (energy independent))
//		defines the decay time (time between energy deposition and photon emission), used for the fast scintillation process (default is 0)
//         "SLOWTIMECONSTANT":		(constant value (energy independent))
//		defines the decay time (time between energy deposition and photon emission), used for the slow scintillation process (default is 0)
//         "FASTSCINTILLATIONRISETIME":	(constant value (energy independent))
//		defines the rise time (time between the start of the emission and the emission peak), used for the fast scintillation process (default is 0)
//         "SLOWSCINTILLATIONRISETIME":	(constant value (energy independent))
//		defines the rise time (time between the start of the emission and the emission peak), used for the slow scintillation process (default is 0)
//         "YIELDRATIO":		(constant value (energy independent))			(obligatory property for scintillator materials, if both "...COMPONENT"s are specified!)
//		defines relative strength of the fast scintillation process as a fraction of total scintillation yield (default is 0)
//
//         "WLSABSLENGTH":		(spectrum (in dependence of the photon energy))		(obligatory property for WLS materials!)
//		defines the absorption length (absorption spectrum) of the material, used for the WLS process (default is infinity, i.e. no WLS process)
//         "WLSCOMPONENT":		(spectrum (in dependence of the photon energy))		(obligatory property for WLS materials!)
//		defines the emission spectrum of the material, used for the WLS process	NOTE: emission spectra are NOT linearly extrapolated between two given points!
//         "WLSTIMECONSTANT":		(constant value (energy independent))
//		defines the decay time (time between absorption and emission), used for the WLS process (default is 0)
//         "WLSMEANNUMBERPHOTONS":	(constant value (energy independent))
//		defines the mean number of photons, emitted for each photon that was absorbed by the WLS material
//		(if specified, the real number of emitted photons is Poisson distributed, else the real number of emitted photons is 1)

//**
// *  Function to define the following material properties of the optical coupling (according to the specifications from the property file):
// *  - refractive index spectrum
// *  - attenuation length / absorption spectrum
// */
void G4OpticalCoupling::DefineMaterialProperties()
{
	G4MaterialPropertyVector * refractiveIndex_Coupling = PropertyTools->GetPropertyDistribution(1.57);//Epoxy
	// G4MaterialPropertyVector * absLength_Coupling = PropertyTools->GetPropertyDistribution(0.25 * CLHEP::mm);
	// G4MaterialPropertyVector * refractiveIndex_Coupling = OpticallyAttachedToVolume_physical->GetLogicalVolume()->GetMaterial()->GetMaterialPropertiesTable()->GetProperty("RINDEX");

	G4MaterialPropertiesTable * MPT_Coupling = new G4MaterialPropertiesTable();
	MPT_Coupling->AddProperty("RINDEX", refractiveIndex_Coupling);
	// MPT_Coupling->AddProperty("ABSLENGTH", absLength_Coupling);
	Material_Coupling->SetMaterialPropertiesTable(MPT_Coupling);
}



void G4OpticalCoupling::SetDefaults()
{
	Material_Coupling = 0;
	Transformation = G4Transform3D();
	Coupling_physical = 0;
}
