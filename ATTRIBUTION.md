# Attribution and provenance

This repository is a modified copy of **GODDeSS** (*Geant4 Objects for Detailed
Detectors with Scintillators and SiPMs*), a Geant4 extension providing object
classes for scintillator tiles, optical fibres and photon detectors.

As most of the code here was written by the GODDeSS
authors, this file records exactly which parts were not, so that credit lands
where it belongs.

## Upstream

- **Project:** GODDeSS
- **Author:** Erik Dietz-Laursonn, Physics Institute 3A, RWTH Aachen University
- **Upstream repository:** <https://git.rwth-aachen.de/3pia/forge/goddess-package>
- **License:** Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported
  (see `source/LICENSE.txt`)

Upstream does not publish a source tree — the repository distributes release
tarballs only (`GODDeSS_1_0.tar.gz` … `GODDeSS_4_3.tar.gz`). There is therefore
no upstream commit history to fork from, which is why this repository begins
with a synthetic import commit rather than sharing ancestry with upstream.

## How this repository is layered

The history is arranged so that each layer is a separate commit and the
boundaries are visible to `git blame` and `git diff`:

| Commit | Contents |
|---|---|
| `80dc574` *Import GODDeSS 4.3 …* | Pristine, unmodified GODDeSS 4.3, extracted from the upstream `GODDeSS_4_3.tar.gz` (sha256 `be96b104580952bc03df85a97914b150bb2123071d0770fe2036ee67f3a7c294`). 125 files. |
| `acc312b` *Import post-4.3 GODDeSS …* | GODDeSS code that never appeared in a release tarball, which reached this project via Matthias Plum's working copy (see [Provenance](#provenance-of-the-post-43-code)). Specifically: the `G4OpticalCoupling` object class — 8 files, ~985 lines, each carrying Erik Dietz-Laursonn's original `author:` / `copyright:` header — the two hooks that integrate it into `GODDeSS_DataStorage` and `GODDeSS_Messenger`, and three one-line `isnan` → `std::isnan` portability fixes. |
| everything after | Modifications by Kiara Carloni (see below). |

To see precisely what was changed locally and by whom:

```bash
# upstream 4.3 -> everything since
git diff 80dc574 main

# only the locally-authored changes (excludes the inherited post-4.3 code)
git diff acc312b main
```

## Provenance of the post-4.3 code

The changes in commit `acc312b` originate from a working copy of GODDESS shared with me (Kiara) and Will Thompson in December 2025 by Matthias Plum from the IceCube collaboration.

That GODDESS code was originally used as part of a simulation of the scintillator panels of the Surface Array Enhancement. Elias Burgfeld, a masters student at the South Dakota School of Mines, contributed significantly to that simulation code as part of his thesis ("Improvement of the Simulation of the IceCube Surface Array Enhancement Scintillation Detector by Muon Flux Measurement").

## Locally-authored changes

Relative to the inherited state, the changes in this repository are:

**New functionality**

- `G4BasicObjects/DataProcessing/HDF5TableWriter.{hh,cc}` — an HDF5 output
  backend. Upstream's `DataProcessing` only accumulates hits in memory and
  leaves persistence to the user's simulation; this adds a reusable columnar
  writer (each table an HDF5 group of equal-length 1-D datasets), with
  run-scoped groups and settable run IDs.
- `G4BasicObjects/G4GeneralParticleSource/ParticleListSource{,Messenger}.{hh,cc}` —
  a primary generator that injects particles from an external list, for driving
  the simulation from air-shower output.
- `G4BasicObjects/G4Fibre` — bent-fibre construction, and an axis-aligned
  bounding-box pre-filter for the fibre-layer overlap tests (a large speedup
  when placing many fibres), plus a guard against an out-of-range access in
  `GrandMotherAndAuntVolumes`.
- Additional material property files under `MaterialProperties/`: Kuraray
  Y11-300 round 1 mm fibre, a 1 mm air-gap fibre coupling, Fermilab
  scintillator, TiO2 wrapping, and an aluminum twin-tile reflector.

**Build and integration**

- `CMakeLists.txt` throughout: headers are exposed via
  `target_include_directories(PUBLIC)` so the package can be consumed as a
  subproject; the build was reorganised to be driven from a parent project.
- Consistent key handling in `GODDeSS_DataStorage`, so energy deposits by
  secondaries are attributed correctly.

**Layout**

- Upstream's `source/Simulation/` was renamed to `source/SimExample/`, and its
  `MaterialProperties/` subdirectory was promoted to `source/MaterialProperties/`.
  These are moves, not rewrites — file contents are unchanged.

## License

This repository inherits GODDeSS's **CC BY-NC-SA 3.0** license. That means any
redistribution or derivative work must credit the original authors, must not be
for commercial purposes, and must be shared under the same license. See
`source/LICENSE.txt` for the full terms.

Note that the sibling `g4sipm` package used alongside this one is licensed
**GPLv3**, which is a separate and stricter set of obligations.

## Citing

If this code contributes to published work, cite GODDeSS and its author rather
than this repository. Consult the upstream
[wiki](https://git.rwth-aachen.de/3pia/forge/goddess-package/wikis/home) for the
preferred citation.
