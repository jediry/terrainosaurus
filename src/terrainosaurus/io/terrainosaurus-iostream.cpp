/*
 * File: terrainosaurus-iostream.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// TODO: Perhaps it would be worthwhile to split 'load', 'analyze' and 'study'
// records for the TerrainSample::LOD...

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import function prototypes
#include "terrainosaurus-iostream.hpp"

// Import parser definitions
#include "TerrainLibraryParser.hpp"
#include "TerrainLibraryLexer.hpp"
#include "MapParser.hpp"
#include "MapLexer.hpp"
#include <antlr/TokenStreamException.hpp>
#include "DEMInterpreter.hpp"

// Import file-related exception definitions
#include <inca/util/UnsupportedOperationException.hpp>
#include <inca/io/FileExceptions.hpp>

// Import raster operator definitions
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/select>

// How many pixels to trim from each side of a DEM file
#define TRIM 30

using namespace inca;
using namespace inca::io;
using namespace inca::raster;
using namespace std;
using namespace terrainosaurus;

typedef TerrainSample::LOD::Feature Feature;


template <typename T, inca::SizeType dim>
void write(std::ostream & os, const inca::raster::MultiArrayRaster<T, dim> & r) {
    typedef inca::raster::MultiArrayRaster<T, dim> Raster;

    // Write the dimensions of the raster
    typename Raster::SizeType sz = r.size();
    typename Raster::IndexArray bs = r.bases();
    typename Raster::IndexArray ex = r.extents();
    os.write((char const *)&bs, sizeof(typename Raster::IndexArray));
    os.write((char const *)&ex, sizeof(typename Raster::IndexArray));

    // Write the raster contents
    if (sz > 0)
        os.write((char const*)r.elements(), sz * sizeof(T));
}
template <typename T, inca::SizeType dim>
void read(std::istream & is, inca::raster::MultiArrayRaster<T, dim> & r) {
    typedef inca::raster::MultiArrayRaster<T, dim> Raster;

    // Set the dimensions of the raster
    typename Raster::IndexArray bs, ex;
    is.read((char*)&bs, sizeof(typename Raster::IndexArray));
    is.read((char*)&ex, sizeof(typename Raster::IndexArray));
    r.setBounds(bs, ex);
    typename Raster::SizeType sz = r.size();

    // Read the raster contents
    if (sz > 0)
        is.read((char*)r.elements(), sz * sizeof(T));
}

template <typename T, inca::SizeType dim>
void write(std::ostream & os, const inca::Array<T, dim> & a) {
    os.write((char const *)a.elements(), dim * sizeof(T));
}
template <typename T, inca::SizeType dim>
void read(std::istream & is, inca::Array<T, dim> & a) {
    is.read((char *)a.elements(), dim * sizeof(T));
}

template <typename T>
void write(std::ostream & os, const std::vector<T> & v) {
    int n = v.size();
    os.write((char const *)&n, sizeof(int));
    if (n > 0)
        os.write((char const *)&v[0], n * sizeof(T));
}    
template <typename T>
void read(std::istream & is, std::vector<T> & v) {
    int n = v.size();
    is.read((char *)&n, sizeof(int));
    v.resize(n);
    if (n > 0)
        is.read((char *)&v[0], n * sizeof(T));
}    

template <typename T, bool cache>
void write(std::ostream & os, const inca::math::Statistics<T, cache> & s) {
    int sz = s.sampleSize();
    os.write((char const *)&sz, sizeof(int));
    write(os, s.histogram());
    write(os, s.centralMoments());
    write(os, s.kStatistics());
    write(os, s.miscellaneous());
}
template <typename T, bool cache>
void read(std::istream & is, inca::math::Statistics<T, cache> & s) {
    int sz;
    is.read((char*)&sz, sizeof(int));
    read(is, s.histogram());
    read(is, s.centralMoments());
    read(is, s.kStatistics());
    read(is, s.miscellaneous());
    s.sampleSize() = sz;
    s.validate();
}

void write(std::ostream & os, const Feature & f) {
    write(os, f.points);
    os.write((char const *)&f.type,   sizeof(Feature::Type));
    os.write((char const *)&f.length, sizeof(scalar_t));
    write(os, f.strengthStats);
    write(os, f.scaleStats);
}
void read(std::istream & is, Feature & f) {
    read(is, f.points);
    is.read((char *)&f.type,   sizeof(Feature::Type));
    is.read((char *)&f.length, sizeof(scalar_t));
    read(is, f.strengthStats);
    read(is, f.scaleStats);
}



// IOstream operators for (de)serializing instances of TerrainLibrary
istream & terrainosaurus::operator>>(istream & is, TerrainLibrary & tl) {
    try {
        // Attempt to parse the stream
        TerrainLibraryLexer lexer(is);
        TerrainLibraryParser parser(lexer);
        parser.sectionList(&tl);

    // Other ANTLR parsing exception
    } catch (const antlr::RecognitionException & e) {
        FileFormatException ffe(e.getFilename(),
                                e.getLine(), e.getColumn(),
                                e.getMessage());
        throw ffe;

    // ANTLR IO exception
    } catch (const antlr::TokenStreamException & e) {
        FileFormatException ffe("", 0, 0, e.getMessage());
        throw ffe;
    }

    return is;
}

ostream & terrainosaurus::operator<<(ostream & os, const TerrainLibrary & tl) {
    throw UnsupportedOperationException("Serialization of TerrainLibraries not implemented");

    // First, write out each TerrainType record
    const TerrainLibrary::TerrainTypeList & types = tl.terrainTypes();
    for (IndexType i = 0; i < IndexType(types.size()); ++i) {
        const TerrainType & tt = *types[i];
        os << "[TerrainType: " << tt.name() << "]\n"
           << "Color = " <<  tt.color() << '\n'
           << '\n';
    }

    // Now, write out each unique TerrainSeam record
    const TerrainLibrary::TerrainSeamMatrix &seams = tl.terrainSeams();
    for (IndexType i = 0; i < IndexType(seams.size()); ++i)
        for (IndexType j = 0; j < i; ++j) {
            const TerrainSeam & ts = *seams[i][j];    // Pick out the seam
            os << "[TerrainSeam: " << types[ts.terrainType1()]->name()
                   << " & " << types[ts.terrainType2()]->name() << "]\n"
               << "Number of Chromosomes = " << ts.numberOfChromosomes() << '\n'
               << "Smoothness = " << ts.smoothness() << '\n'
               << "Mutation Ratio = " << ts.mutationRatio() << '\n'
               << "Crossover Ratio = " << ts.crossoverRatio() << '\n'
               << "Selection Ratio = " << ts.selectionRatio() << '\n'
               << "Aspect Ratio = " << ts.aspectRatio() << '\n'
               << '\n';
        }

    return os;
}


// IOstream operators for (de)serializing instances of Map
istream & terrainosaurus::operator>>(istream & is, Map & m) {
    MapLexer lexer(is);
    MapParser parser(lexer);

    try {
        // Attempt to parse the stream
        parser.terrainMap(&m);

    // Other ANTLR parsing exception
    } catch (const antlr::RecognitionException &e) {
        FileFormatException ffe(parser.getFilename(),
                                e.getLine(), e.getColumn(),
                                e.getMessage());
        throw ffe;

    // ANTLR IO exception
    } catch (const antlr::TokenStreamException &e) {
        FileFormatException ffe(parser.getFilename(), 0, 0, e.getMessage());
        throw ffe;
    }

    return is;
}

ostream & terrainosaurus::operator<<(ostream & os, const Map & m) {
    throw UnsupportedOperationException("Serialization of Maps not implemented");
    // Create the file header comments
    os << "# This file is a Terrainosaurus map\n"
       << '\n';

    // Write out the vertices (intersections)
    os << "# Vertex declarations\n"
       << '\n';

    // Write out the faces (regions)
    os << "# Vertex declarations\n"
       << '\n';

    return os;
}


// IOstream operators for (de)serializing Heightfields
istream & terrainosaurus::operator>>(istream & is, Heightfield & hf) {
    typedef Heightfield::ElementType Scalar;

    Heightfield temp;
    DEMInterpreter dem(temp);

    // Attempt to parse the stream
    dem.parse(is);
    
    // Trim it and store it to the right place
    Pixel       start(TRIM);
    Dimension   size(temp.sizes());
    size -= Dimension(2 * TRIM);

    // FIXME: This is just here until DFT works with all shapes
    size[0] = size[1] = std::min((size[0] / 2) * 2, (size[1] / 2) * 2);

    // Put the loaded data into the correct LOD
    scalar_t resX = scalar_t(dem.resolution[0]);
    scalar_t resY = scalar_t(dem.resolution[1]);
    if (resX != resY) {
        INCA_WARNING("loadFromFile(): DEM has different X and Y "
                     "resolutions -- using X resolution")
    }

    TerrainLOD fileLOD = LODForMetersPerSample(resX);
    INCA_DEBUG("File contains " << fileLOD)

    if (dem.verticalUnits == 1 || dem.resolution[2] != 1.0f) {
        scalar_t scaleFactor = scalar_t(dem.resolution[2]);
        if (dem.verticalUnits == 1) scaleFactor *= 3.25f;
        INCA_DEBUG("Rescaling elevations by factor of " << scaleFactor)
        hf = selectBS(temp, start, size) * scaleFactor;
    } else {
        hf = selectBS(temp, start, size);
    }

    return is;
}

ostream & terrainosaurus::operator<<(ostream & os, const Heightfield & m) {
    throw UnsupportedOperationException("Serialization of Heightfields not implemented");
    return os;
}


// IOstream operators for (de)serializing instances of TerrainSample::LOD
istream & terrainosaurus::operator>>(istream & is, TerrainSample::LOD & ts) {
    // Read the magic header, to make sure this file is what we think it is
    // If the header is not present, we throw an exception
    char header[15];
    is.get(header, 15);
    if (std::string("Terrainosaurus") != header)
        throw FileFormatException("File does not have the correct magic "
                                  "header. Are you sure this is a cache file?");

    // Read the LOD and dimensions
    TerrainLOD lod;
    is.read((char*)&lod, sizeof(TerrainLOD));

    // Warn if this doesn't seem like the right LOD
    if (lod != ts.levelOfDetail())
        INCA_WARNING("Uh oh...stream contains " << lod
                     << ", not expected " << ts.levelOfDetail())

    int filepos;
    try {
        // Read in each of the rasters
        filepos = is.tellg();   read(is, ts._elevations);
        filepos = is.tellg();   read(is, ts._gradients);
        filepos = is.tellg();   read(is, ts._localElevationMeans);
        filepos = is.tellg();   read(is, ts._localGradientMeans);
        filepos = is.tellg();   read(is, ts._localElevationLimits);
        filepos = is.tellg();   read(is, ts._localSlopeLimits);

        // Write out each of the non-raster measurements
        filepos = is.tellg();   read(is, ts._frequencySpectrum);
        filepos = is.tellg();   read(is, ts._globalElevationStatistics);
        filepos = is.tellg();   read(is, ts._globalSlopeStatistics);
        filepos = is.tellg();   read(is, ts._globalEdgeLengthStatistics);
        filepos = is.tellg();   read(is, ts._globalEdgeScaleStatistics);
        filepos = is.tellg();   read(is, ts._globalEdgeStrengthStatistics);

        // Write out each of the feature sets
        filepos = is.tellg();   read(is, ts._peaks);
        filepos = is.tellg();   read(is, ts._edges);
        filepos = is.tellg();   read(is, ts._ridges);

        // We should now be at the end of the file
        INCA_DEBUG("Stream pointer is " << is.tellg())

        // Nothing more to do here...
        ts._loaded = true;
        ts._analyzed = true;
        ts._studied = true;

    } catch (std::exception &) {
        inca::io::FileFormatException e("");
        e << "Cache file ended prematurely (file pointer was " << filepos << ")";
        throw e;
    }
    
    return is;
}
ostream & terrainosaurus::operator<<(ostream & os, const TerrainSample::LOD & ts) {
    // Make sure we have stuff to store first
    ts.ensureStudied();

    // Write the magic header string to the os
    os << "Terrainosaurus";

    // Write the LOD and sizes of this TerrainSample
    TerrainLOD lod = ts.levelOfDetail();
    os.write((char*)&lod, sizeof(TerrainLOD));

    // Write out each of the rasters
    write(os, ts._elevations);
    write(os, ts._gradients);
    write(os, ts._localElevationMeans);
    write(os, ts._localGradientMeans);
    write(os, ts._localElevationLimits);
    write(os, ts._localSlopeLimits);

    // Write out each of the non-raster measurements
    write(os, ts._frequencySpectrum);
    write(os, ts._globalElevationStatistics);
    write(os, ts._globalSlopeStatistics);
    write(os, ts._globalEdgeLengthStatistics);
    write(os, ts._globalEdgeScaleStatistics);
    write(os, ts._globalEdgeStrengthStatistics);

    // Write out each of the feature sets
    write(os, ts._peaks);
    write(os, ts._edges);
    write(os, ts._ridges);

    INCA_DEBUG("Stream pointer os " << os.tellp())
    
    return os;
}
