/**
 * \file    TerrainosaurusApplication.cpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the member functions belonging to the
 *      TerrainosaurusApplication class, defined in
 *      TerrainosaurusApplication.hpp.
 */
 
/* We need some more robust handling for properties. Perhaps string-names,
 * as in Java, though the type-handling problem is a little thornier, since
 * int's are not objects in C++. We need to have default values hard-coded in
 * the App class, or else there needs to be a warning or hard-stop if an unset
 * property is accessed. Something like the properties macros I wrote might work.
 * Also, I need to decide whether to allow them to be accessed by named accessors,
 * or with generic, typed accessors.
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "TerrainosaurusApplication.hpp"
#include <terrainosaurus/genetics/terrain-operations.hpp>
typedef ::terrainosaurus::TerrainosaurusApplication  TApp;

// Import Timer definition
#include <inca/util/Timer>
using namespace inca;

// Import raster operation definitions
#include <inca/raster/operators/select>
#include <inca/raster/algorithms/fill>
using namespace inca::raster;


// Import I/O class definitions
#include <fstream>
#include <terrainosaurus/io/terrainosaurus-iostream.hpp>
#include <terrainosaurus/io/ConfigLexer.hpp>
#include <terrainosaurus/io/ConfigParser.hpp>
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;
using namespace terrainosaurus;

// Import Antlr exception definitions
#include <antlr/TokenStreamException.hpp>


#define STRING_PROPERTY_COUNT  1
#define INTEGER_PROPERTY_COUNT 6
#define SCALAR_PROPERTY_COUNT  16

// Default data file names
#define CACHE_DIR "/home/jediry/tmp/terrainosaurus-cache"
#define DATA_DIR "/home/jediry/Documents/Development/Media/terrainosaurus/data/"
#define CONFIG_FILE     DATA_DIR "terrainosaurus.conf"
#define SOURCE_DIR      DATA_DIR "dem/"
#define DEFAULT_MAP     DATA_DIR "test.map"
#define DEFAULT_TTL     DATA_DIR "test.ttl"
#define MATLAB_FILE     cacheDirectory() + "stats.m"

// Function switches (0 or 1)
#define DISABLE_CACHE       0
#define FORCE_CACHE_WRITE   0
#define BLUR_HISTOGRAM      1

// Cache is always disabled in analyze mode
#if ANALYZE_MODE == 2
    #ifdef DISABLE_CACHE
        #undef DISABLE_CACHE
    #endif
    #define DISABLE_CACHE 1
#endif

// HACK 'd in stuff for loading DEM files and caching them
#include <sys/stat.h>
#include <errno.h>


/*---------------------------------------------------------------------------*
 | Access to the singleton Application instance
 *---------------------------------------------------------------------------*/
TApp & TApp::instance() {
    return dynamic_cast<TApp &>(Application::instance());
}


/*---------------------------------------------------------------------------*
 | Application setup functions
 *---------------------------------------------------------------------------*/
// Constructor
TApp::TerrainosaurusApplication()
    : _stringProperties(STRING_PROPERTY_COUNT),
      _integerProperties(INTEGER_PROPERTY_COUNT),
      _scalarProperties(SCALAR_PROPERTY_COUNT) { }

void TApp::setup(int & argc, char **& argv) {
    // Seed the random number generator
    srand(getSystemClocks());

    // See if the user gave us any useful filenames
    std::string arg, ext;
    while (argc > 1) {
        arg = shift(argc, argv);
        ext = arg.substr(arg.length() - 4);
        if (ext == ".dem")          _terrainFilenames.push_back(arg);
        else if (arg[0] == '@')     _terrainFilenames.push_back(arg);
        else if (ext == ".ttl")     _libraryFilenames.push_back(arg);
        else if (ext == ".map")     _mapFilenames.push_back(arg);
        else
            exit(1, "Unrecognized argument \"" + arg + "\"");
    }

    // Load the configuration file
    try {
        loadConfigFile(CONFIG_FILE);
    } catch (inca::StreamException & e) {
        INCA_ERROR("[" << CONFIG_FILE << "]: " << e)
    }

    // HACK If something wasn't specifed on the command-line, choose a default
//    if (_mapFilenames.size() == 0)
//        _mapFilenames.push_back(DEFAULT_MAP);
    if (_libraryFilenames.size() == 0)
        _libraryFilenames.push_back(DEFAULT_TTL);

    // Load each of the terrain libraries on the command-line
    for (IndexType i = 0; i < IndexType(_libraryFilenames.size()); ++i)
        try {
            loadTerrainLibrary(_libraryFilenames[i]);
        } catch (inca::StreamException & e) {
            INCA_ERROR("[" << _libraryFilenames[i] << "]: " << e)
        }
}

// Put together our user interface
void TApp::construct() {
    // Keep track of how many windows we create here
    int windowCount = 0;

    // Create windows for each terrain on the command-line
    for (IndexType i = 0; i < IndexType(_terrainFilenames.size()); ++i)
        try {
            createTerrainSampleWindow(_terrainFilenames[i]);
            ++windowCount;
        } catch (inca::StreamException & e) {
            INCA_ERROR("[" << _terrainFilenames[i] << "]: " << e)
        }

    // Create windows for each map on the command-line
    for (IndexType i = 0; i < IndexType(_mapFilenames.size()); ++i)
        try {
            createMapEditorWindow(_mapFilenames[i]);
            ++windowCount;
        } catch (inca::StreamException & e) {
            INCA_ERROR("[" << _mapFilenames[i] << "]: " << e)
        }

    // XXX
    if (windowCount == 0) {
        try {
            INCA_INFO("No window specified...creating one")
            IDMap ids(inca::Array<int, 2>(300, 300));
            fill(ids, 4);
            // TODO: compiler *used* to accept this, but was binding non-const ref to
            // the temporary returned by selectBS. Need to rework this so that fill
            // takes its argument by value, and the rasters are OK with that.
//            fill(selectBS(ids, IndexArray(10, 10), SizeArray(50, 50)), IDType(3));
//            fill(selectBS(ids, IndexArray(60, 60), SizeArray(50, 50)), IDType(2));
//            fill(selectBS(ids, IndexArray(120, 120), SizeArray(50, 50)), IDType(1));
            typedef SelectRegionOperatorRaster< IDMap > MapSelection;
            MapSelection sel1 = selectBS(ids, IndexArray(10, 10), SizeArray(50, 50));   fill(sel1, 3);
            MapSelection sel2 = selectBS(ids, IndexArray(60, 60), SizeArray(50, 50));   fill(sel2, 2);
            MapSelection sel3 = selectBS(ids, IndexArray(120, 120), SizeArray(50, 50)); fill(sel3, 1);
            MapRasterizationPtr mr(new MapRasterization(ids, LOD_30m, _lastTerrainLibrary));
            createTerrainSampleWindow(mr);
            ++windowCount;
        } catch (inca::StreamException & e) {
            INCA_ERROR("Creating extra window: " << e)
        }
    }

    // Make sure we did something with all this effort
    if (windowCount == 0)
        exit(1, "No windows created...exiting");
}

#if ANALYZE_MODE == 1

template <typename T, bool C>
void statisticsToMatlab(std::ostream & os,
                        const std::string & variable,
                        const inca::math::Statistics<T, C> & stat) {
    os << variable << ".sampleSize = " << stat.sampleSize() << ";\n"
       << variable << ".mean       = " << stat.mean()       << ";\n"
       << variable << ".stddev     = " << stat.stddev()     << ";\n"
       << variable << ".variance   = " << stat.variance()   << ";\n"
       << variable << ".skewness   = " << stat.skewness()   << ";\n"
       << variable << ".kurtosis   = " << stat.kurtosis()   << ";\n"
       << variable << ".sum        = " << stat.sum()        << ";\n"
       << variable << ".range      = " << stat.range()      << ";\n"
       << variable << ".min        = " << stat.min()        << ";\n"
       << variable << ".max        = " << stat.max()        << ";\n"
       << variable << ".k          = [ ";
    for (int i = 1; i < stat.kStatisticCount(); ++i)
        os << stat.k(i) << " ";
    os << " ];\n"
       << variable << ".moment     = [ ";
    for (int i = 1; i < stat.centralMomentCount(); ++i)
        os << stat.centralMoment(i) << " ";
    os << " ];\n"
       << variable << ".histogram.bucketSize = " << stat.histogramBucketSize() << ";\n"
       << variable << ".histogram.values = [ ";
    for (int i = 0; i < stat.histogramBucketCount(); ++i)
        #if BLUR_HISTOGRAM
            if (i == 0 || i == stat.histogramBucketCount() - 1)
                os << stat.histogramBucket(i) << " ";
            else
                os << ((2 * stat.histogramBucket(i)
                        + stat.histogramBucket(i - 1)
                        + stat.histogramBucket(i + 1)) / 4.0f) << " ";
        #else
            os << stat.histogramBucket(i) << " ";
        #endif
    os << " ];\n"
       << variable << ".histogram.centers = [ ";
    for (int i = 0; i < stat.histogramBucketCount(); ++i)
        os << (stat.min() + (i + 0.5f) * stat.histogramBucketSize()) << " ";
    os << " ];\n"
       << '\n';
       
    // Now also generate the gaussian approximation to the histogram
    os << variable << "_approx = " << variable << ";\n"
       << variable << "_approx.histogram.values = "
                      "normpdf(" << variable << ".histogram.centers, "
                                 << variable << ".mean, "
                                 << variable << ".stddev);\n"
       << variable << "_approx.histogram.values = "
                            << variable << "_approx.histogram.values "
                      " * " << variable << ".sampleSize"
                      " / sum(" << variable << "_approx.histogram.values);\n"
       << '\n';
}

int TApp::main(int & argc, char **& argv) {
    setup(argc, argv);

    TerrainLOD lod = LOD_270m;
    TerrainLibraryPtr tl = _lastTerrainLibrary;
    const TerrainLibrary::LOD & tll = (*tl)[lod];

    // Create a Matlab script file to reflect each statistics object into Matlab
    std::ofstream mFile(MATLAB_FILE);
    mFile << "% Terrainosaurus TerrainLibrary Matlab script\n"
          << '\n'
          << '\n'
          << "tl.tt = {};\n"
          << '\n';
    for (IDType ttid = 1; ttid < IDType(tll.size()); ++ttid) {
        const TerrainType::LOD & ttl = tll.terrainType(ttid);
        
        std::ostringstream ttvar;
        ttvar << "tl.tt{" << ttid << "}";
        mFile << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
              << "% Terrain Type: " << ttl.name() << '\n'
              << '\n'
              << ttvar.str() << " = struct();\n"
              << ttvar.str() << ".name = '" << ttl.name() << "';\n"
              << ttvar.str() << ".ts = {};\n"
              << '\n';

        for (IDType tsid = 0; tsid < IDType(ttl.size()); ++tsid) {
            const TerrainSample::LOD & tsl = ttl.terrainSample(tsid);
            tsl.ensureStudied();
    
            std::ostringstream tsvar;
            tsvar << ttvar.str() << ".ts{" << (tsid + 1) << "}";
            mFile << "%%%%%%%%%%%%\n"
                  << "% TerrainSample: " << tsl.name() << '\n'
                  << '\n'
                  << tsvar.str() << " = struct();\n";
            statisticsToMatlab(mFile, tsvar.str() + ".elevation",
                               tsl.globalElevationStatistics());
            statisticsToMatlab(mFile, tsvar.str() + ".slope",
                               tsl.globalSlopeStatistics());
            mFile << '\n';
        }
    }
    mFile.close();

//    TerrainSamplePtr ts = tl->terrainType(1)->terrainSample(0);
//    ts->ensureStudied(lod);
//    loadAnalysisCache((*ts)[lod]);
//    std::cerr << "Global Elevation Stats" << std::endl
//              << (*ts)[lod].globalElevationStatistics() << std::endl;
    return 0;
    
    
//    SimilarityGA ga((*tl)[lod]);
//    const SimilarityChromosome & best = ga.run();
//    best.setValues();
//    terrainLibraryFitness((*tl)[lod], true);
//    return 0;
}

#elif ANALYZE_MODE == 2
int TApp::main(int & argc, char **& argv) {
    setup(argc, argv);
    
    _lastTerrainLibrary->ensureStudied(LOD_30m);

    return 0;
}
#elif ANALYZE_MODE == 3
int TApp::main(int & argc, char **& argv) {
    setup(argc, argv);

    terrainTypeFitness((*_lastTerrainLibrary)[LOD_270m].terrainType("Florida_Bumpy_Flatland"));    
//    terrainLibraryFitness((*_lastTerrainLibrary)[LOD_270m], true);

    return 0;
}
#endif


/*---------------------------------------------------------------------------*
 | Window management functions
 *---------------------------------------------------------------------------*/
MapEditorWindowPtr
TApp::createMapEditorWindow(const std::string & filename) {
    MapPtr map = loadMap(filename);
    MapEditorWindowPtr win = createMapEditorWindow(map);
    win->setTitle(filename);
    return win;
}
MapEditorWindowPtr
TApp::createMapEditorWindow(MapPtr map) {
    return static_pointer_cast<MapEditorWindow>(add(new MapEditorWindow(map)));
}


TerrainSampleWindowPtr
TApp::createTerrainSampleWindow(const std::string & filename) {
    TerrainSamplePtr ts;
    if (filename[0] == '@') {       // Library reference
        std::string name;
        int index;
        std::string::size_type length = filename.length();
        if (filename[length - 1] == ']') {
            std::string::size_type bracket = filename.rfind('[');
            std::stringstream ss(filename.substr(bracket + 1,
                                                 length - bracket - 2));
            name = filename.substr(1, bracket - 1);
            ss >> index;
        } else {
            name = filename.substr(1);
            index = 0;
        }
        INCA_DEBUG("Library Sample " << name << ", sample " << index)
        ts = _lastTerrainLibrary->terrainType(name)->terrainSample(index);
    } else {
        ts.reset(new TerrainSample(filename));
    }
    TerrainSampleWindowPtr win = createTerrainSampleWindow(ts);
    win->setTitle(filename);
    return win;
}
TerrainSampleWindowPtr TApp::createTerrainSampleWindow(TerrainSamplePtr ts) {
    return static_pointer_cast<TerrainSampleWindow>(add(new TerrainSampleWindow(ts)));
}
TerrainSampleWindowPtr TApp::createTerrainSampleWindow(MapRasterizationPtr mr) {
    TerrainSamplePtr ts(new TerrainSample());
    TerrainSamplePtr ps(new TerrainSample());
    ts->setMapRasterization(mr);
    ps->setMapRasterization(mr);
    return static_pointer_cast<TerrainSampleWindow>(add(new TerrainSampleWindow(ts, ps)));
}

ImageWindowPtr TApp::createImageWindow(const GrayscaleImage & img) {
    return static_pointer_cast<ImageWindow>(add(new ImageWindow(img)));
}
ImageWindowPtr TApp::createImageWindow(const ColorImage & img) {
    return static_pointer_cast<ImageWindow>(add(new ImageWindow(img)));
}


/*---------------------------------------------------------------------------*
 | Data object management functions
 *---------------------------------------------------------------------------*/
// Search for files of different LODs, in the order we would prefer them
TerrainLOD TApp::bestAvailableLOD(const std::string & prefix,
                                  const std::string & suffix,
                                  TerrainLOD preferred) const {
    struct stat dummy;

    // Prefer to downsample from higher LOD, if possible (better quality)
    for (TerrainLOD lod = preferred; lod < TerrainLOD_Overflow; ++lod) {
        std::ostringstream path;
        path << prefix << metersPerSampleForLOD(lod) << suffix;
        if (stat(path.str().c_str(), &dummy) == 0)  // If it exists...
            return lod;
    }
    
    // Upsample, if we absolutely have to
    for (TerrainLOD lod = preferred - 1; lod > TerrainLOD_Underflow; --lod) {
        std::ostringstream path;
        path << prefix << metersPerSampleForLOD(lod) << suffix;
        if (stat(path.str().c_str(), &dummy) == 0)  // If it exists...
            return lod;
    }
    
    // If we found nothing...return overflow
    return TerrainLOD_Overflow;
}
TerrainLOD TApp::bestAvailableElevationMapLOD(const std::string & basename,
                                              TerrainLOD preferred) {
    return bestAvailableLOD(SOURCE_DIR + basename + " (", "m).dem", preferred);
}
TerrainLOD TApp::bestAvailableTerrainTypeMapLOD(const std::string & basename,
                                                TerrainLOD preferred) {
    return bestAvailableLOD(SOURCE_DIR + basename + " (", "m).png", preferred);
}
std::string TApp::elevationMapFilename(const std::string & basename,
                                       TerrainLOD lod) const {
    if (lod != TerrainLOD_Overflow) {
        std::ostringstream ss;
        ss << SOURCE_DIR << basename << " ("
           << metersPerSampleForLOD(lod) << "m).dem";
        return ss.str();
    } else {
        return std::string();
    }
}
std::string TApp::terrainTypeMapFilename(const std::string & basename,
                                         TerrainLOD lod) const {
    if (lod != TerrainLOD_Overflow) {
        std::ostringstream ss;
        ss << SOURCE_DIR << basename << " ("
           << metersPerSampleForLOD(lod) << "m).png";
        return ss.str();
    } else {
        return std::string();
    }
}
std::string TApp::analysisCacheFilename(const std::string & basename,
                                        TerrainLOD lod) const {
    if (lod != TerrainLOD_Overflow) {
        std::ostringstream ss;
        ss << cacheDirectory() << basename << " ("
           << metersPerSampleForLOD(lod) << "m).cache";
        return ss.str();
    } else {
        return std::string();
    }
}


// Throws inca::io::FileAccessException if the file cannot be opened for reading
// Throws inca::io::FileFormatException if the file is syntactically or semantically invalid
MapPtr TApp::loadMap(const std::string & path) {
    MapPtr map(new Map());
    loadMap(map, path);
    return map;
}
void TApp::loadMap(MapPtr m, const std::string & path) {
    INCA_INFO("[" << path << "]: loading Map")

    // Try to open the file and scream if we fail
    std::ifstream file(path.c_str());
    if (! file) {
        FileAccessException e(path);
        e << "Unable to read Map file [" << path
            << "]: does it exist?";
        throw e;
    }

    // Create a new Map object and initialize it
    m->terrainLibrary = _lastTerrainLibrary;
    file >> *m;
    file.close();

    INCA_INFO("[" << path << "]: loading complete "
              "(" << m->vertexCount() << " vertices, "
                  << m->faceCount() << " regions, "
                  << m->edgeCount() << " boundaries)")
}

// Throws inca::io::FileAccessException if the file cannot be opened for writing
void TApp::storeMap(MapConstPtr map, const std::string & path) {
    INCA_INFO("[" << path << "]: storing Map")

    // Try to open the file and scream if we fail
    std::ofstream file(path.c_str());
    if (! file) {
        FileAccessException e(path);
        e << "Unable to write Map file [" << path
          << "]: check directory/file permissions";
        throw e;
    }

    // Write the Map out to the file
    file << *map;
    file.close();

    INCA_INFO("[" << path << "]: storing complete")
}


// Throws inca::io::FileAccessException if the file cannot be opened for reading
// Throws inca::io::FileFormatException if the file is syntactically or semantically invalid
TerrainLibraryPtr TApp::loadTerrainLibrary(const std::string & path) {
    TerrainLibraryPtr lib(new TerrainLibrary());
    loadTerrainLibrary(lib, path);
    return lib;
}
void TApp::loadTerrainLibrary(TerrainLibraryPtr lib, const std::string & path) {
    INCA_INFO("[" << path << "]: loading TerrainLibrary")

    // Try to open the file and scream if we fail
    std::ifstream file(path.c_str());
    if (! file) {
        FileAccessException e(path);
        e << "Unable to read TerrainLibrary file [" << path
          << "]: does it exist?";
        throw e;
    }

    // Initialize the TerrainLibrary object
    file >> *lib;
    file.close();

    // HACK
    _lastTerrainLibrary = lib;

    INCA_INFO("[" << path << "]: loading complete")
}

// Throws inca::io::FileAccessException if the file cannot be opened for writing
void TApp::storeTerrainLibrary(TerrainLibraryConstPtr lib,
                               const std::string & path) {
    INCA_INFO("[" << path << "]: storing TerrainLibrary")

    // Try to open the file and scream if we fail
    std::ofstream file(path.c_str());
    if (! file) {
        FileAccessException e(path);
        e << "Unable to write TerrainLibrary file [" << path
            << "]: check directory/file permissions";
        throw e;
    }

    // Write the TerrainLibrary out to the file
    file << *lib;
    file.close();

    INCA_INFO("[" << path << "]: storing complete")
}


void TApp::loadSourceFiles(TerrainSample & ts) {
    INCA_INFO("[" << ts.name() << "]: trying to load source files")

    // Extract the file base name from the object
    std::string basename = ts.filename();
    if (basename == "") {
        FileAccessException e("");
        e << __FUNCTION__ << "(): TerrainSample does not have a base filename";
        throw e;
    }

    // Find the highest-quality source DEM/map files for the TS
    TerrainLOD demLOD = bestAvailableElevationMapLOD(basename, TerrainLOD::maximum());
    TerrainLOD mapLOD = bestAvailableTerrainTypeMapLOD(basename, TerrainLOD::maximum());

    // Load the DEM file into an IDMap, if we have it        
    Heightfield hf;
    if (demLOD != TerrainLOD_Overflow) {
        // We have a DEM file, so load it.
		std::string path = elevationMapFilename(basename, demLOD);
        std::ifstream file(path.c_str());
        if (! file) {
            FileAccessException e(path);
            e << "Unable to read DEM file [" << path << "]: does it exist?";
            throw e;
        }
        file >> hf;
        file.close();
        ts[demLOD].createFromRaster(hf);
#if FORCE_CACHE_WRITE
        ts[demLOD].ensureStudied();
#endif
        INCA_INFO("Loaded elevation map " << path
                  << " (" << ts[demLOD].sizes() << ')')
    } else {
        inca::io::FileAccessException e(basename);
        e << "No elevation map file found";
        throw e;
    }

    // Load the map file into an IDMap, if we have it        
    IDMap map;
    if (mapLOD != TerrainLOD_Overflow) {
        // We have a map file, so load it.
		std::string path = terrainTypeMapFilename(basename, mapLOD);
        std::ifstream file(path.c_str());
        if (! file) {
            FileAccessException e(path);
            e << "Unable to read map file [" << path << "]: does it exist?";
            throw e;
        }
        throw UnsupportedOperationException("Reading rasterized maps not supported");
        INCA_INFO("Loaded terrain type map " << path)
    }
}
void TApp::loadAnalysisCache(TerrainSample::LOD & tsl) {
    INCA_INFO("[" << tsl.name() << "]: trying to load cache")

    // Extract the file base name from the parent TS object
    std::string basename = tsl.object().filename();
    if (basename == "") {
        FileAccessException e("");
        e << __FUNCTION__ << "(): TerrainSample does not have a base filename";
        throw e;
    }

    // Find the source DEM/map files for the TS, and the cache for this LOD
    TerrainLOD cacheLOD = tsl.levelOfDetail();
    TerrainLOD demLOD   = bestAvailableElevationMapLOD(basename, cacheLOD);
	TerrainLOD mapLOD   = bestAvailableTerrainTypeMapLOD(basename, cacheLOD);
    std::string cacheFilename = analysisCacheFilename(basename,  cacheLOD);
	std::string demFilename   = elevationMapFilename(basename,   demLOD);
	std::string mapFilename   = terrainTypeMapFilename(basename, mapLOD);
    
    // Check each file's existence & timestamp
    struct stat demStat, mapStat, cacheStat;
    int ds = stat(demFilename.c_str(),   &demStat);
    int ms = stat(mapFilename.c_str(),   &mapStat);
    int cs = stat(cacheFilename.c_str(), &cacheStat);
    
    std::string reason;
    bool cacheValid = true;
    bool cacheExpired = false;

#if DISABLE_CACHE
    cacheValid = false;
    reason = "CACHE DISABLED";
#endif
    
    // If the cache does not exist, then we can't load it, can we?
    if (cacheValid && cs != 0) {
        cacheValid   = false;
        cacheExpired = false;
        reason = "cache file '" + cacheFilename + "' does not exist";
    }

    // If the cache exists, but one of the other files is newer, then we
    // delete it and act as though it had never been
    if (cacheValid && (ds == 0) && (demStat.st_mtime > cacheStat.st_mtime)) {
        cacheValid   = false;
        cacheExpired = true;
        reason = "elevation map is newer than cache";
    }
    if (cacheValid && (ms == 0) && (mapStat.st_mtime > cacheStat.st_mtime)) {
        cacheValid   = false;
        cacheExpired = true;
        reason = "terrain type map is newer than cache";
    }

    // Delete the cache file if it's too old
    if (cacheExpired) {
        INCA_INFO("[" << tsl.name() << "]: Deleting expired cache")
        unlink(cacheFilename.c_str());
    }
    
    // Load the cache file, if we've got it, and scream bloody murder if not
    if (cacheValid) {
        std::ifstream file(cacheFilename.c_str(), std::ios::binary);
        file.exceptions(std::ios::badbit | std::ios::eofbit);
        if (! file) {
            FileAccessException e(cacheFilename);
            e << "Unable to read TerrainSample::LOD cache file ["
              << cacheFilename << "]: does it exist?";
            throw e;
        }
        file >> tsl;
        file.close();
        INCA_INFO("[" << tsl.name() << "]: cache load successful")

    } else {
        INCA_INFO("[" << tsl.name() << "]: " << reason)
        throw FileAccessException(cacheFilename, reason);
    }
}

// Throws inca::io::FileAccessException if the file cannot be opened for writing
void TApp::storeAnalysisCache(const TerrainSample::LOD & tsl) {
    INCA_INFO("[" << tsl.name() << "]: storing analysis cache")

    // Figure out where we'll put it    
    std::string basename = tsl.object().filename();
    if (basename == "") {
        FileAccessException e("");
        e << __FUNCTION__ << "(): TerrainSample does not have a base filename";
        throw e;
    }
    std::string cacheFilename = analysisCacheFilename(basename,
                                                      tsl.levelOfDetail());

    // Try to open the file and scream if we fail
    std::ofstream file(cacheFilename.c_str(), std::ios::binary);
    file.exceptions(std::ios::badbit | std::ios::eofbit);
    if (! file) {
        FileAccessException e(cacheFilename);
        e << "Unable to write cache file '" << cacheFilename << "': "
             "check directory/file permissions";
        throw e;
    }

    // Write the TerrainSample::LOD out to the file
    file << tsl;
    file.close();

    INCA_INFO("[" << tsl.name() << "]: cache store successful")
}


// Throws inca::io::FileAccessException if the file cannot be opened for reading
// Throws inca::io::FileFormatException if the file is syntactically or semantically invalid
void TApp::loadConfigFile(const std::string & path) {
    INCA_INFO("[" << path << "]: loading configuration settings")

    // Antlr, I hate you.
    setCacheDirectory(CACHE_DIR);

    setBoundaryGAPopulationSize(60);
    setBoundaryGAEvolutionCycles(20);
    setBoundaryGASelectionRatio(0.85f);
    setBoundaryGAElitismRatio(0.20f);
    setBoundaryGAMutationProbability(0.30f);
    setBoundaryGAMutationRatio(0.25f);
    setBoundaryGACrossoverProbability(0.10f);
    setBoundaryGACrossoverRatio(0.10f);
    setBoundaryGAMaxAbsoluteAngle(inca::math::PI<scalar_t>() * 0.4f);

    setHeightfieldGAPopulationSize(15);
    setHeightfieldGAEvolutionCycles(15);
    setHeightfieldGASelectionRatio(0.75f);
    setHeightfieldGAElitismRatio(0.2f);
    setHeightfieldGAMutationProbability(0.5f);
    setHeightfieldGAMutationRatio(0.5f);
    setHeightfieldGACrossoverProbability(0.5f);
    setHeightfieldGACrossoverRatio(0.5f);
    setHeightfieldGAGeneSize(16);
    setHeightfieldGAOverlapFactor(0.2f);
    setHeightfieldGAMaxCrossoverWidth(4);
    setHeightfieldGAMaxVerticalScale(1.1f);
    setHeightfieldGAMaxVerticalOffset(50.0f);

    return;     // EARLY RETURN. I hate you Antlr.
    

    // Try to open the file and scream if we fail
    std::ifstream file(path.c_str());
    if (! file) {
        FileAccessException e(path);
        e << "Unable to read configuration file [" << path
          << "]: does it exist?";
        throw e;
    }

    // Read the configuration file
    ConfigLexer lexer(file);
    ConfigParser parser(lexer);
    try {
        // Attempt to parse the stream
        parser.sectionList(this);

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

    file.close();

    INCA_INFO("[" << path << "]: loading complete")
}


// Throws inca::io::FileAccessException if the file cannot be opened for writing
void TApp::storeCofigFile(const std::string & path) const {
    INCA_INFO("[" << path << "]: storing configuration settings")

    // Try to open the file and scream if we fail
    std::ofstream file(path.c_str());
    file.exceptions(std::ios::badbit | std::ios::eofbit);
    if (! file) {
        FileAccessException e(path);
        e << "Unable to write confi file '" << path << "': "
             "check directory/file permissions";
        throw e;
    }

    // Write the application configuration out to the file
//  ???
    file.close();

    INCA_INFO("[" << path << "]: configuration settings store successful")
}

TerrainLibraryPtr TApp::lastTerrainLibrary() { return _lastTerrainLibrary; }


/*---------------------------------------------------------------------------*
 | Configuration settings functions
 *---------------------------------------------------------------------------*/
// String properties
#define CACHE_DIR_PATH  0

// Integer properties
#define HF_POP_SZ       0
#define HF_EVO_CYCLES   1
#define HF_GENE_SZ      2
#define HF_XO_W         3
#define BDR_POP_SZ      4
#define BDR_EVO_CYCLES  5

// Scalar properties
#define HF_SEL_R        0
#define HF_ELT_R        1
#define HF_MUT_P        2
#define HF_MUT_R        3
#define HF_XO_P         4
#define HF_XO_R         5
#define HF_OVR_F        6
#define HF_V_SCALE      7
#define HF_V_OFF        8
#define BDR_SEL_R       9
#define BDR_ELT_R       10
#define BDR_MUT_P       11
#define BDR_MUT_R       12
#define BDR_XO_P        13
#define BDR_XO_R        14
#define BDR_A_A         15


// Application settings
const std::string & TApp::cacheDirectory() const { return _stringProperties[CACHE_DIR_PATH]; }
void TApp::setCacheDirectory(const std::string & d) { _stringProperties[CACHE_DIR_PATH] = d; }

// Heightfield GA settings
int TApp::heightfieldGAPopulationSize() const { return _integerProperties[HF_POP_SZ]; }
int TApp::heightfieldGAEvolutionCycles() const { return _integerProperties[HF_EVO_CYCLES]; }
scalar_t TApp::heightfieldGASelectionRatio() const { return _scalarProperties[HF_SEL_R]; }
scalar_t TApp::heightfieldGAElitismRatio() const { return _scalarProperties[HF_ELT_R]; }
scalar_t TApp::heightfieldGAMutationProbability() const { return _scalarProperties[HF_MUT_P]; }
scalar_t TApp::heightfieldGAMutationRatio() const { return _scalarProperties[HF_MUT_R]; }
scalar_t TApp::heightfieldGACrossoverProbability() const { return _scalarProperties[HF_XO_P]; }
scalar_t TApp::heightfieldGACrossoverRatio() const { return _scalarProperties[HF_XO_R]; }
int TApp::heightfieldGAGeneSize() const { return _integerProperties[HF_GENE_SZ]; }
scalar_t TApp::heightfieldGAOverlapFactor() const { return _scalarProperties[HF_OVR_F]; }
int TApp::heightfieldGAMaxCrossoverWidth() const { return _integerProperties[HF_XO_W]; }
scalar_t TApp::heightfieldGAMaxVerticalScale() const { return _scalarProperties[HF_V_SCALE]; }
scalar_t TApp::heightfieldGAMaxVerticalOffset() const { return _scalarProperties[HF_V_OFF]; }
void TApp::setHeightfieldGAPopulationSize(int n) { _integerProperties[HF_POP_SZ] = n; }
void TApp::setHeightfieldGAEvolutionCycles(int n) { _integerProperties[HF_EVO_CYCLES] = n; }
void TApp::setHeightfieldGASelectionRatio(scalar_t s) { _scalarProperties[HF_SEL_R] = s; }
void TApp::setHeightfieldGAElitismRatio(scalar_t s) { _scalarProperties[HF_ELT_R] = s; }
void TApp::setHeightfieldGAMutationProbability(scalar_t s) { _scalarProperties[HF_MUT_P] = s; }
void TApp::setHeightfieldGAMutationRatio(scalar_t s) { _scalarProperties[HF_MUT_R] = s; }
void TApp::setHeightfieldGACrossoverProbability(scalar_t s) { _scalarProperties[HF_XO_P] = s; }
void TApp::setHeightfieldGACrossoverRatio(scalar_t s) { _scalarProperties[HF_XO_R] = s; }
void TApp::setHeightfieldGAGeneSize(int n) { _integerProperties[HF_GENE_SZ] = n; }
void TApp::setHeightfieldGAOverlapFactor(scalar_t s) { _scalarProperties[HF_OVR_F] = s; }
void TApp::setHeightfieldGAMaxCrossoverWidth(int n) { _integerProperties[HF_XO_W] = n; }
void TApp::setHeightfieldGAMaxVerticalScale(scalar_t s) { _scalarProperties[HF_V_SCALE] = s; }
void TApp::setHeightfieldGAMaxVerticalOffset(scalar_t s) { _scalarProperties[HF_V_OFF] = s; }

// Boundary GA settings
int TApp::boundaryGAPopulationSize() const { return _integerProperties[BDR_POP_SZ]; }
int TApp::boundaryGAEvolutionCycles() const { return _integerProperties[BDR_EVO_CYCLES]; }
scalar_t TApp::boundaryGASelectionRatio() const { return _scalarProperties[BDR_SEL_R]; }
scalar_t TApp::boundaryGAElitismRatio() const { return _scalarProperties[BDR_ELT_R]; }
scalar_t TApp::boundaryGAMutationProbability() const { return _scalarProperties[BDR_MUT_P]; }
scalar_t TApp::boundaryGAMutationRatio() const { return _scalarProperties[BDR_MUT_R]; }
scalar_t TApp::boundaryGACrossoverProbability() const { return _scalarProperties[BDR_XO_P]; }
scalar_t TApp::boundaryGACrossoverRatio() const { return _scalarProperties[BDR_XO_R]; }
scalar_t TApp::boundaryGAMaxAbsoluteAngle() const { return _scalarProperties[BDR_A_A]; }
void TApp::setBoundaryGAPopulationSize(int n) { _integerProperties[BDR_POP_SZ] = n; }
void TApp::setBoundaryGAEvolutionCycles(int n) { _integerProperties[BDR_EVO_CYCLES] = n; }
void TApp::setBoundaryGASelectionRatio(scalar_t s) { _scalarProperties[BDR_SEL_R] = s; }
void TApp::setBoundaryGAElitismRatio(scalar_t s) { _scalarProperties[BDR_ELT_R] = s; }
void TApp::setBoundaryGAMutationProbability(scalar_t s) { _scalarProperties[BDR_MUT_P] = s; }
void TApp::setBoundaryGAMutationRatio(scalar_t s) { _scalarProperties[BDR_MUT_R] = s; }
void TApp::setBoundaryGACrossoverProbability(scalar_t s) { _scalarProperties[BDR_XO_P] = s; }
void TApp::setBoundaryGACrossoverRatio(scalar_t s) { _scalarProperties[BDR_XO_R] = s; }
void TApp::setBoundaryGAMaxAbsoluteAngle(scalar_t s) { _scalarProperties[BDR_A_A] = s; }


// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::TerrainosaurusApplication);
