/**
 * \file    TerrainosaurusApplication.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainosaurusApplication class represents the application itself,
 *      and is responsible for processing command-line arguments and for
 *      creating and coordinating the various windows and other structures
 *      that do the real work.
 *
 * HACK: This will have to do for now, but a better system for handling files
 *      is needed. Things it doesn't do now that it ought to:
 *          * resolve relative to base directories and/or current directory
 *          * handle absolute paths
 *          * choose load-cache/load-dem more intelligently
 *          * when to load a different cache rather than loading the file...
 */

#ifndef TERRAINOSAURUS_APPLICATION
#define TERRAINOSAURUS_APPLICATION

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the fluid simulation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainosaurusApplication;
};


// Import superclass definition
#include <inca/ui.hpp>

// Import window and data object definitions
#include <terrainosaurus/ui/MapEditorWindow.hpp>
#include <terrainosaurus/ui/TerrainSampleWindow.hpp>
#include <terrainosaurus/ui/ImageWindow.hpp>
#include <terrainosaurus/data/TerrainLibrary.hpp>
#include <terrainosaurus/data/Map.hpp>
#include <terrainosaurus/data/TerrainSample.hpp>


// Import container definitions
#include <vector>

// define this to enable an alternate application mode, for analyzing the
// terrain library
//      0 -- normal operation
//      1 -- print matlab file?
//      2 -- calculate aggregate TT and TL variances
//      3 -- calculate library self-fitness
#define ANALYZE_MODE 0


// TerrainosaurusApplication is an instance of an Application, using the
// GUI toolkit specified in terrainosaurus-common.h
class terrainosaurus::TerrainosaurusApplication
        : public inca::ui::Application {
/*---------------------------------------------------------------------------*
 | Access to the singleton Application instance
 *---------------------------------------------------------------------------*/
public:
    static TerrainosaurusApplication & instance();


/*---------------------------------------------------------------------------*
 | Application setup functions
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    explicit TerrainosaurusApplication();
    

    #if ANALYZE_MODE
        // Don't create a GUI, but instead analyze the TL
        int main(int & argc, char **& argv);
    #endif

    // Get command-line arguments and set up the terrainscape
    void setup(int & argc, char **& argv);

    // Put together our user interface
    void construct();

protected:
    // Filenames to load, given on the commandline
    std::vector<std::string>    _terrainFilenames,  // .dem heightfield files
                                _libraryFilenames,  // .ttl terrain lib files
                                _mapFilenames;      // .map files


/*---------------------------------------------------------------------------*
 | Window management functions
 *---------------------------------------------------------------------------*/
public:
    MapEditorWindowPtr createMapEditorWindow(const std::string & filename);
    MapEditorWindowPtr createMapEditorWindow(MapPtr map);
    TerrainSampleWindowPtr createTerrainSampleWindow(const std::string & filename);
    TerrainSampleWindowPtr createTerrainSampleWindow(TerrainSamplePtr ts);
    TerrainSampleWindowPtr createTerrainSampleWindow(MapRasterizationPtr mr);
    ImageWindowPtr createImageWindow(const GrayscaleImage & img);
    ImageWindowPtr createImageWindow(const ColorImage & img);

protected:
    // Existing windows
    std::vector<MapEditorWindowPtr>     _mapEditorWindows;
    std::vector<TerrainSampleWindowPtr> _terrainSampleWindows;
    std::vector<ImageWindowPtr>         _imageWindows;


/*---------------------------------------------------------------------------*
 | Data object management functions
 *---------------------------------------------------------------------------*/
public:
    // File functions
    TerrainLOD bestAvailableLOD(const std::string & prefix,
                                const std::string & suffix,
                                TerrainLOD preferred) const;
    TerrainLOD bestAvailableElevationMapLOD(const std::string & basename,
                                            TerrainLOD preferred);
    TerrainLOD bestAvailableTerrainTypeMapLOD(const std::string & basename,
                                              TerrainLOD preferred);
    std::string elevationMapFilename(const std::string & path, TerrainLOD lod) const;
    std::string terrainTypeMapFilename(const std::string & path, TerrainLOD lod) const;
    std::string analysisCacheFilename(const std::string & path, TerrainLOD lod) const;

    MapPtr loadMap(const std::string & path);
    void loadMap(MapPtr m, const std::string & path);
    void storeMap(MapConstPtr m, const std::string & path);

    TerrainLibraryPtr loadTerrainLibrary(const std::string & path);
    void loadTerrainLibrary(TerrainLibraryPtr tl, const std::string & path);
    void storeTerrainLibrary(TerrainLibraryConstPtr tl, const std::string & path);

    void loadSourceFiles(TerrainSample & tsl);

    void loadAnalysisCache(TerrainSample::LOD & tsl);
    void storeAnalysisCache(const TerrainSample::LOD & tsl);

    void loadConfigFile(const std::string & path);
    void storeCofigFile(const std::string & path) const;

    TerrainLibraryPtr lastTerrainLibrary();
    
protected:
    // HACK
    TerrainLibraryPtr _lastTerrainLibrary;


/*---------------------------------------------------------------------------*
 | Configuration settings functions
 *---------------------------------------------------------------------------*/
public:
    // Application settings
    const std::string & cacheDirectory() const;
    void setCacheDirectory(const std::string & d);
    
    // Heightfield GA settings
    int heightfieldGAPopulationSize() const;
    int heightfieldGAEvolutionCycles() const;
    scalar_t heightfieldGASelectionRatio() const;
    scalar_t heightfieldGAElitismRatio() const;
    scalar_t heightfieldGAMutationProbability() const;
    scalar_t heightfieldGAMutationRatio() const;
    scalar_t heightfieldGACrossoverProbability() const;
    scalar_t heightfieldGACrossoverRatio() const;
    int heightfieldGAGeneSize() const;
    scalar_t heightfieldGAOverlapFactor() const;
    int heightfieldGAMaxCrossoverWidth() const;
    scalar_t heightfieldGAMaxVerticalScale() const;
    scalar_t heightfieldGAMaxVerticalOffset() const;
    void setHeightfieldGAPopulationSize(int s);
    void setHeightfieldGAEvolutionCycles(int s);
    void setHeightfieldGASelectionRatio(scalar_t s);
    void setHeightfieldGAElitismRatio(scalar_t s);
    void setHeightfieldGAMutationProbability(scalar_t s);
    void setHeightfieldGAMutationRatio(scalar_t s);
    void setHeightfieldGACrossoverProbability(scalar_t s);
    void setHeightfieldGACrossoverRatio(scalar_t s);
    void setHeightfieldGAGeneSize(int s);
    void setHeightfieldGAOverlapFactor(scalar_t s);
    void setHeightfieldGAMaxCrossoverWidth(int s);
    void setHeightfieldGAMaxVerticalScale(scalar_t s);
    void setHeightfieldGAMaxVerticalOffset(scalar_t s);

    // Boundary GA settings
    int boundaryGAPopulationSize() const;
    int boundaryGAEvolutionCycles() const;
    scalar_t boundaryGASelectionRatio() const;
    scalar_t boundaryGAElitismRatio() const;
    scalar_t boundaryGAMutationProbability() const;
    scalar_t boundaryGAMutationRatio() const;
    scalar_t boundaryGACrossoverProbability() const;
    scalar_t boundaryGACrossoverRatio() const;
    scalar_t boundaryGAMaxAbsoluteAngle() const;
    void setBoundaryGAPopulationSize(int s);
    void setBoundaryGAEvolutionCycles(int s);
    void setBoundaryGASelectionRatio(scalar_t s);
    void setBoundaryGAElitismRatio(scalar_t s);
    void setBoundaryGAMutationProbability(scalar_t s);
    void setBoundaryGAMutationRatio(scalar_t s);
    void setBoundaryGACrossoverProbability(scalar_t s);
    void setBoundaryGACrossoverRatio(scalar_t s);
    void setBoundaryGAMaxAbsoluteAngle(scalar_t s);

protected:
    std::vector<std::string>    _stringProperties;
    std::vector<int>            _integerProperties;
    std::vector<scalar_t>       _scalarProperties;
};

#endif
