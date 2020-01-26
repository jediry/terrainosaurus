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

// Import parser definitions (generated into the build tree)
#include <terrainosaurus/io/TerrainLibraryLexer.h>
#include <terrainosaurus/io/TerrainLibraryParser.h>
#include <terrainosaurus/io/TerrainLibraryListener.h>
#include <terrainosaurus/io/MapLexer.h>
#include <terrainosaurus/io/MapParser.h>
#include <terrainosaurus/io/MapListener.h>
#include "PrimitivesBaseListener.hpp"
#include "FailFastErrorListener.hpp"
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


namespace terrainosaurus {

std::string chomp(const std::string& s) {
    if (s.rfind('\n') + 1 == s.length())
        return s.substr(0, s.length() - 1);
    else
        return s;
}

}

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


class TerrainLibraryBuilder : public TPrimitivesBaseListener<TerrainLibraryListener, TerrainLibraryParser>
{
private:
    enum class TerrainTypePropertyID {
        Color,
    };

    enum class TerrainSeamPropertyID {
        Smoothness,
        AspectRatio,
    };

public:
    TerrainLibraryBuilder(TerrainLibrary & tl, TerrainLibraryParser & parser) noexcept
      : _terrainLibrary(tl), _parser(parser) { }

    void enterSectionList(TerrainLibraryParser::SectionListContext *ctx) override { }
    void exitSectionList(TerrainLibraryParser::SectionListContext *ctx) override { }

    void enterTerrainTypeSection(TerrainLibraryParser::TerrainTypeSectionContext *ctx) override {
        // Scream like hell if we've already got one
        auto tt = ctx->string()->getText();
        TerrainTypePtr ttp = _terrainLibrary.terrainType(tt);
        if (ttp != NULL) {
            FileFormatException e(_parser.getSourceName(),
                                  static_cast<int>(ctx->start->getLine()),
                                  static_cast<int>(ctx->start->getCharPositionInLine()));
            e << "TerrainType \"" << tt << "\" has already been created";
            throw e;
        }

        // Make a new one with this name
        _currentTT = _terrainLibrary.addTerrainType(tt);
    }
    void exitTerrainTypeSection(TerrainLibraryParser::TerrainTypeSectionContext *ctx) override {
        assert(_currentTT != nullptr);

        // Make sure we have everything we need
        if (_mapTTPropertyIDToParserRuleContext.find(TerrainTypePropertyID::Color) == _mapTTPropertyIDToParserRuleContext.end()) {
            FileFormatException e(_parser.getSourceName(),
                                  static_cast<int>(ctx->start->getLine()),
                                  static_cast<int>(ctx->start->getCharPositionInLine()));
            e << "TerrainType \"" << _currentTT->name() << "\" does not have a represetative color assigned to it";
            throw e;
        }

        if (_currentTT->size() == 0) {
            FileFormatException e(_parser.getSourceName(),
                                  static_cast<int>(ctx->start->getLine()),
                                  static_cast<int>(ctx->start->getCharPositionInLine()));
            e << "TerrainType \"" << _currentTT->name() << "\" has no terrain samples assigned to it";
            throw e;
        }

        _currentTT = nullptr;
        _mapTTPropertyIDToParserRuleContext.clear();
    }

    void enterTerrainSeamSection(TerrainLibraryParser::TerrainSeamSectionContext *ctx) override {
        std::string tt1 = ctx->string(0)->getText();
        std::string tt2 = ctx->string(1)->getText();

        // Make sure the associated TerrainType records exist
        TerrainTypePtr tt1p = _terrainLibrary.terrainType(tt1);
        TerrainTypePtr tt2p = _terrainLibrary.terrainType(tt2);
        if (tt1p == NULL || tt2p == NULL) {
            FileFormatException e(_parser.getSourceName(),
                                  static_cast<int>(ctx->start->getLine()),
                                  static_cast<int>(ctx->start->getCharPositionInLine()));
            e << "TerrainSeam between \"" << tt1 << "\" and \"" << tt2
              << "\" cannot be created: ";
            if (tt1p == NULL && tt2p == NULL) { // Both are missing
                e << "neither TerrainType record exists";
            } else if (tt1p == NULL) {          // The first is missing
                e << "a TerrainType record for \"" << tt1 << "\" does not exist";
            } else {                            // The second is missing
                e << "a TerrainType record for \"" << tt2 << "\" does not exist";
            }
            throw e;
        }

        // Look up the TS from its TTs
        TerrainSeamPtr ts = _terrainLibrary.terrainSeam(tt1p->terrainTypeID(),
                                                        tt2p->terrainTypeID());

        // Make sure we've not already initialized this TerrainSeam
        auto it = _mapTSToParserRuleContext.find(ts);
        if (it != _mapTSToParserRuleContext.end()) {
            FileFormatException e(_parser.getSourceName(),
                                  static_cast<int>(ctx->start->getLine()),
                                  static_cast<int>(ctx->start->getCharPositionInLine()));
            e << "TerrainSeam between \"" << tt1 << "\" and \""
              << tt2 << "\" has already been initialized (at line "<< it->second->start->getLine() << ")";
            throw e;
        }


        // This is the new current TS; mark it as "initialized"
        _currentTS = ts;
        _mapTSToParserRuleContext[_currentTS] = ctx;
    }
    void exitTerrainSeamSection(TerrainLibraryParser::TerrainSeamSectionContext * ctx) override {
        _mapTSPropertyIDToParserRuleContext.clear();
    }

    void enterTerrainColorAssignment(TerrainLibraryParser::TerrainColorAssignmentContext * /*ctx*/) override { }
    void exitTerrainColorAssignment(TerrainLibraryParser::TerrainColorAssignmentContext * ctx) override {
        assert(_currentTT != nullptr);
        SetUniqueProperty<TerrainTypePropertyID::Color>(ctx, [&](const Color & c) noexcept { _currentTT->setColor(c); });
    }

    void enterTerrainSampleAssignment(TerrainLibraryParser::TerrainSampleAssignmentContext * /*ctx*/) override { }
    void exitTerrainSampleAssignment(TerrainLibraryParser::TerrainSampleAssignmentContext * ctx) override {
        assert(_currentTT != nullptr);

        // Only add this sample if it's not a duplicate
        auto path = ctx->string()->getText();
        for (SizeType i = 0; i < _currentTT->size(); ++i)
            // TODO: Use C++14 filesystem support here to handle path normalization
            if (_currentTT->terrainSample(i)->filename() == path) {
                INCA_WARNING("Ignoring duplicate terrain sample \"" << path << "\" (line " << ctx->start->getLine() << ")");
                return;
            }

        _currentTT->addTerrainSample(TerrainSamplePtr(new TerrainSample(ctx->string()->getText())));
    }

    void enterSmoothnessAssignment(TerrainLibraryParser::SmoothnessAssignmentContext * /*ctx*/) override { }
    void exitSmoothnessAssignment(TerrainLibraryParser::SmoothnessAssignmentContext * ctx) override {
        assert(_currentTS != nullptr);
        SetUniqueProperty<TerrainSeamPropertyID::Smoothness>(ctx, [&](double value) noexcept { _currentTS->smoothness = value; });
    }

    void enterAspectRatioAssignment(TerrainLibraryParser::AspectRatioAssignmentContext * /*ctx*/) override { }
    void exitAspectRatioAssignment(TerrainLibraryParser::AspectRatioAssignmentContext * ctx) override {
        assert(_currentTS != nullptr);
        SetUniqueProperty<TerrainSeamPropertyID::AspectRatio>(ctx, [&](double value) noexcept { _currentTS->aspectRatio = value; });
    }

private:

    template<TerrainTypePropertyID ID, typename ParserRuleContextType, typename AssignOp>
    void SetUniqueProperty(ParserRuleContextType * ctx, const AssignOp & assignProperty)
    {
        DoSetUniqueProperty<TerrainTypePropertyID, ID>(ctx, _mapTTPropertyIDToParserRuleContext, assignProperty);
    }

    template<TerrainSeamPropertyID ID, typename ParserRuleContextType, typename AssignOp>
    void SetUniqueProperty(ParserRuleContextType * ctx, const AssignOp & assignProperty)
    {
        DoSetUniqueProperty<TerrainSeamPropertyID, ID>(ctx, _mapTSPropertyIDToParserRuleContext, assignProperty);
    }

    template<typename IDType, IDType ID, typename ParserRuleContextType, typename AssignOp>
    void DoSetUniqueProperty(ParserRuleContextType * ctx,
                             std::unordered_map<IDType, antlr4::ParserRuleContext *> & mapPropertyIDToParserRuleContext,
                             const AssignOp & assignProperty)
    {
        antlr4::ParserRuleContext*& pCtx = mapPropertyIDToParserRuleContext[ID];

        if (pCtx != nullptr) {
            // Oooh...already saw this one
            FileFormatException ffe(_parser.getSourceName(),
                                    static_cast<int>(ctx->start->getLine()),
                                    static_cast<int>(ctx->start->getCharPositionInLine()));
            ffe << "Property " << chomp(pCtx->getText()) << " occurs more than once (previous assignment at line " << pCtx->start->getLine() << ", " << pCtx->start->getCharPositionInLine() << ")";
            throw ffe;
        }

        pCtx = ctx;

        // Invoke the setter to actually store the value for the property
        assignProperty(ctx->value);
    }

    TerrainLibrary & _terrainLibrary;
    TerrainLibraryParser & _parser;
    TerrainTypePtr _currentTT { nullptr };
    TerrainSeamPtr _currentTS { nullptr };
    std::unordered_map<TerrainSeamPtr, antlr4::ParserRuleContext*> _mapTSToParserRuleContext;
    std::unordered_map<TerrainTypePropertyID, antlr4::ParserRuleContext*> _mapTTPropertyIDToParserRuleContext;
    std::unordered_map<TerrainSeamPropertyID, antlr4::ParserRuleContext*> _mapTSPropertyIDToParserRuleContext;
};

// IOstream operators for (de)serializing instances of TerrainLibrary
istream & terrainosaurus::operator>>(istream & is, TerrainLibrary & tl) {
    antlr4::ANTLRInputStream stream(is);
    TerrainLibraryLexer lexer(&stream);
    antlr4::CommonTokenStream tokens(&lexer);
    TerrainLibraryParser parser(&tokens);
    FailFastErrorListener errorListener;
    parser.addErrorListener(&errorListener);

    // Attempt to parse the stream
    auto* tree = parser.sectionList();
    TerrainLibraryBuilder builder(tl, parser);
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&builder, tree);

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


class MapBuilder : public TPrimitivesBaseListener<MapListener, MapParser>
{
public:
    MapBuilder(Map & map, MapParser & parser) noexcept : _map(map), _parser(parser) { }

    void enterTerrainMap(MapParser::TerrainMapContext * /*ctx*/) override { }
    void exitTerrainMap(MapParser::TerrainMapContext * /*ctx*/) override { }

    void enterVertexRecord(MapParser::VertexRecordContext * /*ctx*/) override { }
    void exitVertexRecord(MapParser::VertexRecordContext * ctx) override {
        _map.createVertex(Point2D(ctx->x->value, ctx->y->value));
    }

    void enterFaceRecord(MapParser::FaceRecordContext * /*ctx*/) override {
    }
    void exitFaceRecord(MapParser::FaceRecordContext * ctx) override {
        if (_currentTT == nullptr) {    // Ensure that a TT has been set
            FileFormatException ffe(_parser.getSourceName(),
                                    static_cast<int>(ctx->start->getLine()),
                                    static_cast<int>(ctx->start->getCharPositionInLine()));
            ffe << "A TerrainType must be selected before faces may be created";
            throw ffe;
        }
        std::vector<Map::VertexPtr> vertices;
        for (const auto * id : ctx->integer()) {
            // Look up the Vertex with that ID (remember file starts at 1, vector at 0)
            Map::VertexPtr v = _map.vertex(id->value - 1);
            if (v == nullptr) {                 // Ensure that the Vertex does exist
                FileFormatException ffe(_parser.getSourceName(),
                                        static_cast<int>(ctx->start->getLine()),
                                        static_cast<int>(ctx->start->getCharPositionInLine()));
                ffe << "Vertex \"" << id->value << "\" does not exist";
                throw ffe;
            }
            vertices.push_back(v);      // Stick it in the list
        }
        _map.createFace(vertices.begin(), vertices.end(), _currentTT);
    }

    void enterTerrainTypeRecord(MapParser::TerrainTypeRecordContext * /*ctx*/) override { }
    void exitTerrainTypeRecord(MapParser::TerrainTypeRecordContext * ctx) override {
        TerrainTypePtr ttp = _map.terrainLibrary->terrainType(ctx->ID()->getSymbol()->getText());
        if (ttp == nullptr) {
            FileFormatException ffe(_parser.getSourceName(),
                                    static_cast<int>(ctx->start->getLine()),
                                    static_cast<int>(ctx->start->getCharPositionInLine()));
            ffe << "TerrainType \"" << ctx->ID()->getSymbol()->getText() << "\" does not exist";
            throw ffe;
        }

        INCA_DEBUG("Selecting active terrain type \"" << ttp->name() << "\"");

        // If we made it here, then the TT we want exists. Yay!
        _currentTT = ttp;
    }

private:
    Map & _map;
    MapParser & _parser;
    TerrainTypePtr _currentTT { nullptr };
};

// IOstream operators for (de)serializing instances of Map
istream & terrainosaurus::operator>>(istream & is, Map & m) {
    antlr4::ANTLRInputStream stream(is);
    MapLexer lexer(&stream);
    antlr4::CommonTokenStream tokens(&lexer);
    MapParser parser(&tokens);
    FailFastErrorListener errorListener;
    parser.addErrorListener(&errorListener);

    // Attempt to parse the stream
    auto* tree = parser.terrainMap();
    MapBuilder builder(m, parser);
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&builder, tree);

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
