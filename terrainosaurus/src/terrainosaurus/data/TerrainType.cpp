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


/*****************************************************************************
 * LOD specialization for TerrainType
 *****************************************************************************/
 // Default constructor
TerrainType::LOD::LOD()
    : LODBase<TerrainType>() { }

// Constructor linking back to TerrainType
TerrainType::LOD::LOD(TerrainTypePtr tt, TerrainLOD lod)
    : LODBase<TerrainType>(tt, lod) { }


// Access to related LOD objects
TerrainLibrary::LOD & TerrainType::LOD::terrainLibrary() {
    return (*object().terrainLibrary())[levelOfDetail()];
}
const TerrainLibrary::LOD & TerrainType::LOD::terrainLibrary() const {
    return (*object().terrainLibrary())[levelOfDetail()];
}
TerrainSample::LOD & TerrainType::LOD::terrainSample(IndexType i) {
    return (*object().terrainSample(i))[levelOfDetail()];
}
const TerrainSample::LOD & TerrainType::LOD::terrainSample(IndexType i) const {
    return (*object().terrainSample(i))[levelOfDetail()];
}
TerrainSample::LOD & TerrainType::LOD::randomTerrainSample() {
    return (*object().randomTerrainSample())[levelOfDetail()];
}
const TerrainSample::LOD & TerrainType::LOD::randomTerrainSample() const {
    return (*object().randomTerrainSample())[levelOfDetail()];
}


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
void TerrainType::LOD::ensureLoaded() const {
    if (! loaded()) {
        for (IndexType i = 0; i < size(); ++i)
            terrainSample(i).ensureLoaded();
        _loaded = true;
    }
}
void TerrainType::LOD::ensureAnalyzed() const {
    if (! analyzed()) {
        for (IndexType i = 0; i < size(); ++i)
            terrainSample(i).ensureAnalyzed();
        _analyzed = true;
    }
}


/*---------------------------------------------------------------------------*
 | Access to parent TerrainType properties
 *---------------------------------------------------------------------------*/
SizeType TerrainType::LOD::size() const {
    return object().size();
}
IDType TerrainType::LOD::terrainTypeID() const {
    return object().terrainTypeID();
}
const std::string & TerrainType::LOD::name() const {
    return object().name();
}
const Color & TerrainType::LOD::color() const {
    return object().color();
}


/*****************************************************************************
 * TerrainType class for representing a logical terrain class
 *****************************************************************************/
/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Constructor
TerrainType::TerrainType(TerrainLibraryPtr lib, IDType id, const std::string &nm)
        : _terrainLibrary(lib), _terrainTypeID(id), _name(nm), _color() { }


/*---------------------------------------------------------------------------*
 | Property accessor functions
 *---------------------------------------------------------------------------*/
// Unique TerrainType ID
IDType TerrainType::terrainTypeID() const     { return _terrainTypeID; }
void TerrainType::setTerrainTypeID(IDType id) { _terrainTypeID = id; }

// TerrainType name
const std::string & TerrainType::name() const    { return _name; }
void TerrainType::setName(const std::string & n) { _name = n; }

// Representative color
const Color & TerrainType::color() const    { return _color; }
void TerrainType::setColor(const Color & c) { _color = c; }

// Associated TerrainLibrary
TerrainLibraryPtr TerrainType::terrainLibrary() {
    return _terrainLibrary;
}
TerrainLibraryConstPtr TerrainType::terrainLibrary() const {
    return _terrainLibrary;
}
void TerrainType::setTerrainLibrary(TerrainLibraryPtr tl) {
    _terrainLibrary = tl;
}

// List of associated TerrainSamples
SizeType TerrainType::size() const {
    return _terrainSamples.size();
}
TerrainSamplePtr TerrainType::terrainSample(IndexType i) {
    return _terrainSamples[i];
}
TerrainSampleConstPtr TerrainType::terrainSample(IndexType i) const {
    return _terrainSamples[i];
}
TerrainSamplePtr TerrainType::randomTerrainSample() {
    RandomUniform<IndexType> randomIndex(0, _terrainSamples.size() - 1);
    return _terrainSamples[randomIndex()];
}
TerrainSampleConstPtr TerrainType::randomTerrainSample() const {
    RandomUniform<IndexType> randomIndex(0, _terrainSamples.size() - 1);
    return _terrainSamples[randomIndex()];
}
void TerrainType::addTerrainSample(TerrainSamplePtr ts) {
    ts->setIndex(_terrainSamples.size());
    ts->setTerrainType(shared_from_this());
    _terrainSamples.push_back(ts);
}
