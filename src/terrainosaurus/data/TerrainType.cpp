/*
 * File: TerrainType.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "TerrainType.hpp"
using namespace terrainosaurus;

// Import random number generator
#include <inca/math/generator/RandomUniform>
using inca::math::RandomUniform;

// Convenient aliases for long names
typedef TerrainType                 TT;
typedef TT::LOD                     TTL;
typedef TTL::DistributionMatcher    DM;


/*****************************************************************************
 * LOD specialization for TerrainType
 *****************************************************************************/
 // Default constructor
TTL::LOD()
    : LODBase<TerrainType>() { }

// Constructor linking back to TerrainType
TTL::LOD(TerrainTypePtr tt, TerrainLOD lod)
    : LODBase<TerrainType>(tt, lod) { }


// Access to related LOD objects
TerrainLibrary::LOD & TTL::terrainLibrary() {
    return (*object().terrainLibrary())[levelOfDetail()];
}
const TerrainLibrary::LOD & TTL::terrainLibrary() const {
    return (*object().terrainLibrary())[levelOfDetail()];
}
TerrainSample::LOD & TTL::terrainSample(IndexType i) {
    return (*object().terrainSample(i))[levelOfDetail()];
}
const TerrainSample::LOD & TTL::terrainSample(IndexType i) const {
    return (*object().terrainSample(i))[levelOfDetail()];
}
TerrainSample::LOD & TTL::randomTerrainSample() {
    return (*object().randomTerrainSample())[levelOfDetail()];
}
const TerrainSample::LOD & TTL::randomTerrainSample() const {
    return (*object().randomTerrainSample())[levelOfDetail()];
}


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
void TTL::analyze() {
    INCA_INFO("Analyzing TerrainType(" << name() << ")")

    // Make sure we're starting from scratch
    _elevationDistribution.reset();
    _slopeDistribution.reset();
    _edgeLengthDistribution.reset();
    _edgeScaleDistribution.reset();
    _edgeStrengthDistribution.reset();

    // Make sure all of the constituent samples are analyzed, then feed their
    // stats into the Borg collective
    for (IndexType i = 0; i < size(); ++i) {
        const TerrainSample::LOD & ts = terrainSample(i);
        ts.ensureAnalyzed();
        _elevationDistribution.examine(ts.globalElevationStatistics());
        _slopeDistribution.examine(ts.globalSlopeStatistics());
        _edgeLengthDistribution.examine(ts.globalEdgeLengthStatistics());
        _edgeScaleDistribution.examine(ts.globalEdgeScaleStatistics());
        _edgeStrengthDistribution.examine(ts.globalEdgeStrengthStatistics());
    }    

    // Analyze what we've seen so we can say Yay/Nay later
    const TerrainLibrary::LOD & tl = terrainLibrary();
    _elevationDistribution.analyze(0.9f,
                                   tl.defaultElevationVariances(),
                                   tl.libraryElevationVariances());
    _slopeDistribution.analyze(0.9f,
                               tl.defaultSlopeVariances(),
                               tl.librarySlopeVariances());
    _edgeLengthDistribution.analyze(0.9f,
                                    tl.defaultEdgeLengthVariances(),
                                    tl.libraryEdgeLengthVariances());
    _edgeScaleDistribution.analyze(0.9f,
                                   tl.defaultEdgeScaleVariances(),
                                   tl.libraryEdgeScaleVariances());
    _edgeStrengthDistribution.analyze(0.9f,
                                      tl.defaultEdgeStrengthVariances(),
                                      tl.libraryEdgeStrengthVariances());


    // Calculate the aggregate agreement and the individual weights
    scalar_t sumAgreement = _elevationDistribution.agreement()
                          + _slopeDistribution.agreement()
                          + _edgeLengthDistribution.agreement()
                          + _edgeScaleDistribution.agreement()
                          + _edgeStrengthDistribution.agreement();
    _elevationWeight    = _elevationDistribution.agreement()    / sumAgreement;
    _slopeWeight        = _slopeDistribution.agreement()        / sumAgreement;
    _edgeLengthWeight   = _edgeLengthDistribution.agreement()   / sumAgreement;
    _edgeScaleWeight    = _edgeScaleDistribution.agreement()    / sumAgreement;
    _edgeStrengthWeight = _edgeStrengthDistribution.agreement() / sumAgreement;

    _agreement = sumAgreement / 4;

    INCA_DEBUG("Results of analyzing TerrainType(" << name() << ")")
    INCA_DEBUG("  Aggregate agreement:  " << _agreement)
    INCA_DEBUG("  Elevation weight:     " << _elevationWeight)
    INCA_DEBUG("    " << _elevationDistribution)
    INCA_DEBUG("  Slope weight:         " << _slopeWeight)
    INCA_DEBUG("    " << _slopeDistribution)
    INCA_DEBUG("  Edge length weight:   " << _edgeLengthWeight)
    INCA_DEBUG("    " << _edgeLengthDistribution)
    INCA_DEBUG("  Edge scale weight:   " << _edgeScaleWeight)
    INCA_DEBUG("    " << _edgeScaleDistribution)
    INCA_DEBUG("  Edge strength weight: " << _edgeStrengthWeight)
    INCA_DEBUG("    " << _edgeStrengthDistribution)

    _analyzed = true;
}

void TTL::ensureLoaded() const {
    if (! loaded()) {
        for (IndexType i = 0; i < size(); ++i)
            terrainSample(i).ensureLoaded();
        _loaded = true;
    }
}
void TTL::ensureAnalyzed() const {
    if (! analyzed()) {
        ensureLoaded();
        const_cast<TTL *>(this)->analyze();
        _analyzed = true;
    }
}
void TTL::ensureStudied() const {
    if (! studied()) {       
        ensureAnalyzed();
        for (IndexType i = 0; i < size(); ++i)
            terrainSample(i).ensureStudied();
        _studied = true;
    }
}


/*---------------------------------------------------------------------------*
 | Statistical distribution matching
 *---------------------------------------------------------------------------*/
const DM & TTL::elevationDistribution() const {
    ensureAnalyzed();
    return _elevationDistribution;
}
const DM & TTL::slopeDistribution() const {
    ensureAnalyzed();
    return _slopeDistribution;
}
const DM & TTL::edgeLengthDistribution() const {
    ensureAnalyzed();
    return _edgeLengthDistribution;
}
const DM & TTL::edgeScaleDistribution() const {
    ensureAnalyzed();
    return _edgeScaleDistribution;
}
const DM & TTL::edgeStrengthDistribution() const {
    ensureAnalyzed();
    return _edgeStrengthDistribution;
}

scalar_t TTL::elevationWeight() const {
    ensureAnalyzed();
    return _elevationWeight;
}
scalar_t TTL::slopeWeight() const {
    ensureAnalyzed();
    return _slopeWeight;
}
scalar_t TTL::edgeLengthWeight() const {
    ensureAnalyzed();
    return _edgeLengthWeight;
}
scalar_t TTL::edgeScaleWeight() const {
    ensureAnalyzed();
    return _edgeScaleWeight;
}
scalar_t TTL::edgeStrengthWeight() const {
    ensureAnalyzed();
    return _edgeStrengthWeight;
}

scalar_t TTL::agreement() const {
    ensureAnalyzed();
    return _agreement;
}


/*---------------------------------------------------------------------------*
 | Access to parent TerrainType properties
 *---------------------------------------------------------------------------*/
SizeType TTL::size() const {
    return object().size();
}
IDType TTL::terrainTypeID() const {
    return object().terrainTypeID();
}
const std::string & TTL::name() const {
    return object().name();
}
const Color & TTL::color() const {
    return object().color();
}


/*****************************************************************************
 * TerrainType class for representing a logical terrain class
 *****************************************************************************/
/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Constructor
TT::TerrainType(TerrainLibraryPtr lib, IDType id, const std::string &nm)
        : _terrainLibrary(lib), _terrainTypeID(id), _name(nm), _color() { }


/*---------------------------------------------------------------------------*
 | Property accessor functions
 *---------------------------------------------------------------------------*/
// Unique TerrainType ID
IDType TT::terrainTypeID() const     { return _terrainTypeID; }
void TT::setTerrainTypeID(IDType id) { _terrainTypeID = id; }

// TerrainType name
const std::string & TT::name() const    { return _name; }
void TT::setName(const std::string & n) { _name = n; }

// Representative color
const Color & TT::color() const    { return _color; }
void TT::setColor(const Color & c) { _color = c; }

// Associated TerrainLibrary
TerrainLibraryPtr TT::terrainLibrary() {
    return _terrainLibrary;
}
TerrainLibraryConstPtr TT::terrainLibrary() const {
    return _terrainLibrary;
}
void TT::setTerrainLibrary(TerrainLibraryPtr tl) {
    _terrainLibrary = tl;
}

// List of associated TerrainSamples
SizeType TT::size() const {
    return _terrainSamples.size();
}
TerrainSamplePtr TT::terrainSample(IndexType i) {
    return _terrainSamples[i];
}
TerrainSampleConstPtr TT::terrainSample(IndexType i) const {
    return _terrainSamples[i];
}
TerrainSamplePtr TT::randomTerrainSample() {
    RandomUniform<IndexType> randomIndex(0, _terrainSamples.size() - 1);
    return _terrainSamples[randomIndex()];
}
TerrainSampleConstPtr TT::randomTerrainSample() const {
    RandomUniform<IndexType> randomIndex(0, _terrainSamples.size() - 1);
    return _terrainSamples[randomIndex()];
}
void TT::addTerrainSample(TerrainSamplePtr ts) {
    ts->setIndex(_terrainSamples.size());
    ts->setTerrainType(shared_from_this());
    _terrainSamples.push_back(ts);
}
