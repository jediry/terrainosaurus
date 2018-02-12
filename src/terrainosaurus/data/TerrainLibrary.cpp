/*
 * File: TerrainLibrary.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *
 * Implementation note:
 *      TerrainTypes are stored in a simple linear vector. TerrainSeams,
 *      on the other hand, are stored in a triangular "matrix" (a vector of
 *      vectors), with the TerrainSeam for a given combination of
 *      TerrainTypes being stored with the greater numeric id of the constituent
 *      types giving the major index and the lesser id giving the index within
 *      the vector at that major index:
 *
 *                   major
 *            |  0   1   2   3
 *    m     --+-----------------  'X' indicates NULL, since TerrainTypes
 *    i     0 |  X  0-1 0-2 0-3   don't have seams with themselves
 *    n     1 |      X  1-2 1-3
 *    o     2 |          X  2-3
 *    r     3 |              X
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "TerrainLibrary.hpp"
using namespace terrainosaurus;

// Import application definition (so we know when we're in ANALYZE_MODE
#include <terrainosaurus/TerrainosaurusApplication.hpp>

// Import random number generator
#include <inca/math/generator/RandomUniform>
using inca::math::RandomUniform;

// Convenient aliases for long names
typedef TerrainLibrary              TL;
typedef TL::LOD                     TLL;
typedef TLL::DistributionMatcher    DM;
typedef TLL::VarianceArray          VA;


/*****************************************************************************
 * LOD specialization for TerrainLibrary
 *****************************************************************************/
 // Default constructor
TLL::LOD()
    : LODBase<TerrainLibrary>() { }

// Constructor linking back to TerrainLibrary
TLL::LOD(TerrainLibraryPtr tl, TerrainLOD lod)
    : LODBase<TerrainLibrary>(tl, lod) { }


// Access to related LOD objects
TerrainType::LOD & TLL::terrainType(IndexType index) {
    return (*object().terrainType(index))[levelOfDetail()];
}
const TerrainType::LOD & TLL::terrainType(IndexType index) const {
    return (*object().terrainType(index))[levelOfDetail()];
}
TerrainType::LOD & TLL::terrainType(const std::string & name) {
    return (*object().terrainType(name))[levelOfDetail()];
}
const TerrainType::LOD & TLL::terrainType(const std::string & name) const {
    return (*object().terrainType(name))[levelOfDetail()];
}
TerrainType::LOD & TLL::randomTerrainType() {
    return (*object().randomTerrainType())[levelOfDetail()];
}
const TerrainType::LOD & TLL::randomTerrainType() const {
    return (*object().randomTerrainType())[levelOfDetail()];
}


/*---------------------------------------------------------------------------*
 | Loading & analysis
 *---------------------------------------------------------------------------*/
void TLL::analyze() {
#if ANALYZE_MODE == 2
    // We'll use these arrays to build up weighted sums of mapping curve
    // variances, so we need to make sure they're zero'd out.
    for (IndexType c = 0; c < DM::componentCount; ++c) {
        _defaultElevationVariances[c]    = 0;
        _defaultSlopeVariances[c]        = 0;
        _defaultEdgeLengthVariances[c]   = 0;
        _defaultEdgeScaleVariances[c]    = 0;
        _defaultEdgeStrengthVariances[c] = 0;
    }
            
    // This keeps track of how many TS's belong to non-singular TT's (i.e.,
    // with more than one TS). We'll need this sum at the end to finish the
    // weighted average calculation.
    int nonSingularSampleCount = 0;

    // Iterate over every TT
    for (IndexType ttid = 0; ttid < size(); ++ttid) {
        const TerrainType::LOD & tt = terrainType(ttid);
        tt.ensureAnalyzed();
        SizeType sampleCount = tt.size();

        // Fold all constituent TS's into the library-wide stats
        for (IndexType tsid = 0; tsid < sampleCount; ++tsid) {
            const TerrainSample::LOD & ts = tt.terrainSample(tsid);
            const DM::Statistics & elevStats  = ts.globalElevationStatistics();
            const DM::Statistics & slopeStats = ts.globalSlopeStatistics();
            const DM::Statistics & eLenStats = ts.globalEdgeLengthStatistics();
            const DM::Statistics & eScStats = ts.globalEdgeScaleStatistics();
            const DM::Statistics & eStrStats = ts.globalEdgeStrengthStatistics();
            
            // Fold the TS's stats into the library-wide DMs
            _elevationDistribution.examine(elevStats);
            _slopeDistribution.examine(slopeStats);
            _edgeLengthDistribution.examine(eLenStats);
            _edgeScaleDistribution.examine(eScStats);
            _edgeStrengthDistribution.examine(eStrStats);
        }

        // If the TT is non-singular, then we also want incorporate its
        // aggregate distribution matching curve variances into the
        // library-wide average (used for singular TT's). This is a
        // weighted average, with more samples => TT weighted more.
        if (sampleCount > 1) {
            nonSingularSampleCount += sampleCount;
            for (IndexType c = 0; c < DM::componentCount; ++c) {
                _defaultElevationVariances[c] +=
                    tt.elevationDistribution().curveVariance(c) * sampleCount;
                _defaultSlopeVariances[c] +=
                    tt.slopeDistribution().curveVariance(c) * sampleCount;
                _defaultEdgeLengthVariances[c] +=
                    tt.edgeLengthDistribution().curveVariance(c) * sampleCount;
                _defaultEdgeScaleVariances[c] +=
                    tt.edgeScaleDistribution().curveVariance(c) * sampleCount;
                _defaultEdgeStrengthVariances[c] +=
                    tt.edgeStrengthDistribution().curveVariance(c) * sampleCount;
            }
        }
    }
    
    // Finish the calculation of the library DM's ("agreement" is meaningless
    // in this case, but we don't use it, so big deal).
    _elevationDistribution.analyze(0.9f, VA(1), VA(1));
    _slopeDistribution.analyze(0.9f, VA(1), VA(1));
    _edgeLengthDistribution.analyze(0.9f, VA(1), VA(1)); 
    _edgeScaleDistribution.analyze(0.9f, VA(1), VA(1));
    _edgeStrengthDistribution.analyze(0.9f, VA(1), VA(1));

    // Copy over the variances
    _libraryElevationVariances    = _elevationDistribution.curveVariances();
    _librarySlopeVariances        = _slopeDistribution.curveVariances();
    _libraryEdgeLengthVariances   = _edgeLengthDistribution.curveVariances();
    _libraryEdgeScaleVariances    = _edgeScaleDistribution.curveVariances();
    _libraryEdgeStrengthVariances = _edgeStrengthDistribution.curveVariances();

    // Finish the weighted average of library-wide variances
    for (IndexType c = 0; c < DM::componentCount; ++c) {
        _defaultElevationVariances[c]    /= nonSingularSampleCount;
        _defaultSlopeVariances[c]        /= nonSingularSampleCount;
        _defaultEdgeLengthVariances[c]   /= nonSingularSampleCount;
        _defaultEdgeScaleVariances[c]    /= nonSingularSampleCount;
        _defaultEdgeStrengthVariances[c] /= nonSingularSampleCount;
    }
    
    INCA_DEBUG("Default elevation variances: " << _defaultElevationVariances)
    INCA_DEBUG("Default slope variances: "     << _defaultSlopeVariances)
    INCA_DEBUG("Default edgeLen variances: "   << _defaultEdgeLengthVariances)
    INCA_DEBUG("Default edgeScl variances: "   << _defaultEdgeScaleVariances)
    INCA_DEBUG("Default edgeStr variances: "   << _defaultEdgeStrengthVariances)

    INCA_DEBUG("Library elevation variances: " << _libraryElevationVariances)
    INCA_DEBUG("Library slope variances: "     << _librarySlopeVariances)
    INCA_DEBUG("Library edgeLen variances: "   << _libraryEdgeLengthVariances)
    INCA_DEBUG("Library edgeScl variances: "   << _libraryEdgeScaleVariances)
    INCA_DEBUG("Library edgeStr variances: "   << _libraryEdgeStrengthVariances)

#else
    switch (levelOfDetail()) {
    case LOD_810m:
        // Store the default variances
        _defaultElevationVariances[0]    = 2.48967e+006f;
        _defaultElevationVariances[1]    = 68969.8f;
        _defaultElevationVariances[2]    = 1.74482f;
        _defaultElevationVariances[3]    = 4.5054f;
        _defaultSlopeVariances[0]        = 0.0159924f;
        _defaultSlopeVariances[1]        = 0.00345138f;
        _defaultSlopeVariances[2]        = 0.81387f;
        _defaultSlopeVariances[3]        = 5.26281f;
        _defaultEdgeLengthVariances[0]   = 69.2324f;
        _defaultEdgeLengthVariances[1]   = 64.9471f;
        _defaultEdgeLengthVariances[2]   = 9.56126f;
        _defaultEdgeLengthVariances[3]   = 81.8732f;
        _defaultEdgeScaleVariances[0]    = 0.0474623f;
        _defaultEdgeScaleVariances[1]    = 0.0925191f;
        _defaultEdgeScaleVariances[2]    = 11.3535f;
        _defaultEdgeScaleVariances[3]    = 559.777f;
        _defaultEdgeStrengthVariances[0] = 3.94204e+009f;
        _defaultEdgeStrengthVariances[1] = 9.10872e+010f;
        _defaultEdgeStrengthVariances[2] = 5.78749f;
        _defaultEdgeStrengthVariances[3] = 156.445f;

        _libraryElevationVariances[0]    = 8.18938e+008f;
        _libraryElevationVariances[1]    = 2.14651e+006f;
        _libraryElevationVariances[2]    = 11.5058f;
        _libraryElevationVariances[3]    = 85.087f;
        _librarySlopeVariances[0]        = 0.737842f;
        _librarySlopeVariances[1]        = 0.17208f;
        _librarySlopeVariances[2]        = 9.7112f;
        _librarySlopeVariances[3]        = 75.7864f;
        _libraryEdgeLengthVariances[0]   = 213.975f;
        _libraryEdgeLengthVariances[1]   = 250.2f;
        _libraryEdgeLengthVariances[2]   = 29.0119f;
        _libraryEdgeLengthVariances[3]   = 218.356f;
        _libraryEdgeScaleVariances[0]    = 0.241273f;
        _libraryEdgeScaleVariances[1]    = 0.655456f;
        _libraryEdgeScaleVariances[2]    = 144.679f;
        _libraryEdgeScaleVariances[3]    = 12309.2f;
        _libraryEdgeStrengthVariances[0] = 1.44523e+011f;
        _libraryEdgeStrengthVariances[1] = 3.642e+012f;
        _libraryEdgeStrengthVariances[2] = 94.0744f;
        _libraryEdgeStrengthVariances[3] = 4255.14f;
        
        break;
        
    case LOD_270m:
        // Store the default variances
        _defaultElevationVariances[0]    = 2.55147e+006f;
        _defaultElevationVariances[1]    = 61710.2f;
        _defaultElevationVariances[2]    = 1.62497f;
        _defaultElevationVariances[3]    = 5.07635f;
        _defaultSlopeVariances[0]        = 0.0598152f;
        _defaultSlopeVariances[1]        = 0.00707744f;
        _defaultSlopeVariances[2]        = 0.868394f;
        _defaultSlopeVariances[3]        = 13.1644f;
        _defaultEdgeLengthVariances[0]   = 11.2203f;
        _defaultEdgeLengthVariances[1]   = 28.073f;
        _defaultEdgeLengthVariances[2]   = 4.85663f;
        _defaultEdgeLengthVariances[3]   = 202.73f;
        _defaultEdgeScaleVariances[0]    = 0.010159f;
        _defaultEdgeScaleVariances[1]    = 0.00471464f;
        _defaultEdgeScaleVariances[2]    = 0.517704f;
        _defaultEdgeScaleVariances[3]    = 4.22359f;
        _defaultEdgeStrengthVariances[0] = 1.37479e+008f;
        _defaultEdgeStrengthVariances[1] = 2.66459e+008f;
        _defaultEdgeStrengthVariances[2] = 6.30632f;
        _defaultEdgeStrengthVariances[3] = 329.037f;

        _libraryElevationVariances[0]    = 8.36541e+008f;
        _libraryElevationVariances[1]    = 2.243e+006f;
        _libraryElevationVariances[2]    = 17.8038f;
        _libraryElevationVariances[3]    = 169.705f;
        _librarySlopeVariances[0]        = 2.99354f;
        _librarySlopeVariances[1]        = 0.726376f;
        _librarySlopeVariances[2]        = 26.4006f;
        _librarySlopeVariances[3]        = 744.281f;
        _libraryEdgeLengthVariances[0]   = 115.17f;
        _libraryEdgeLengthVariances[1]   = 875.528f;
        _libraryEdgeLengthVariances[2]   = 19.067f;
        _libraryEdgeLengthVariances[3]   = 1158.59f;
        _libraryEdgeScaleVariances[0]    = 0.068295f;
        _libraryEdgeScaleVariances[1]    = 0.0563025f;
        _libraryEdgeScaleVariances[2]    = 8.07123f;
        _libraryEdgeScaleVariances[3]    = 133.961f;
        _libraryEdgeStrengthVariances[0] = 3.98846e+009f;
        _libraryEdgeStrengthVariances[1] = 7.22486e+009f;
        _libraryEdgeStrengthVariances[2] = 28.7741f;
        _libraryEdgeStrengthVariances[3] = 2119.64f;

        break;
        
    case LOD_90m:
        // Store the default variances
        _defaultElevationVariances[0]    = 2.57432e+006f;
        _defaultElevationVariances[1]    = 58793.9f;
        _defaultElevationVariances[2]    = 1.62268f;
        _defaultElevationVariances[3]    = 5.9172f;
        _defaultSlopeVariances[0]        = 0.133614f;
        _defaultSlopeVariances[1]        = 0.0210205f;
        _defaultSlopeVariances[2]        = 0.953902f;
        _defaultSlopeVariances[3]        = 22.5653f;
        _defaultEdgeLengthVariances[0]   = 4.01647f;
        _defaultEdgeLengthVariances[1]   = 11.2122f;
        _defaultEdgeLengthVariances[2]   = 6.5584f;
        _defaultEdgeLengthVariances[3]   = 709.385f;
        _defaultEdgeScaleVariances[0]    = 0.00691748f;
        _defaultEdgeScaleVariances[1]    = 0.00217038f;
        _defaultEdgeScaleVariances[2]    = 0.263018f;
        _defaultEdgeScaleVariances[3]    = 1.56668f;
        _defaultEdgeStrengthVariances[0] = 3.18248e+008f;
        _defaultEdgeStrengthVariances[1] = 8.8755e+009f;
        _defaultEdgeStrengthVariances[2] = 46.3341f;
        _defaultEdgeStrengthVariances[3] = 9214.05f;

        _libraryElevationVariances[0]    = 8.4327e+008f;
        _libraryElevationVariances[1]    = 2.27198e+006f;
        _libraryElevationVariances[2]    = 18.3972f;
        _libraryElevationVariances[3]    = 183.552f;
        _librarySlopeVariances[0]        = 7.85064f;
        _librarySlopeVariances[1]        = 2.82791f;
        _librarySlopeVariances[2]        = 94.2921f;
        _librarySlopeVariances[3]        = 7388.01f;
        _libraryEdgeLengthVariances[0]   = 16.1331f;
        _libraryEdgeLengthVariances[1]   = 861.253f;
        _libraryEdgeLengthVariances[2]   = 210.815f;
        _libraryEdgeLengthVariances[3]   = 30706.2f;
        _libraryEdgeScaleVariances[0]    = 0.102723f;
        _libraryEdgeScaleVariances[1]    = 0.0246934f;
        _libraryEdgeScaleVariances[2]    = 3.19918f;
        _libraryEdgeScaleVariances[3]    = 13.1849f;
        _libraryEdgeStrengthVariances[0] = 6.3307e+012f;
        _libraryEdgeStrengthVariances[1] = 1.94556e+016f;
        _libraryEdgeStrengthVariances[2] = 12324.3f;
        _libraryEdgeStrengthVariances[3] = 4.17062e+007f;

        break;

    case LOD_30m:
        // Store the default variances
        _defaultElevationVariances[0]    = 2.5853e+006f;
        _defaultElevationVariances[1]    = 58112.3f;
        _defaultElevationVariances[2]    = 1.60765f;
        _defaultElevationVariances[3]    = 6.20289f;
        _defaultSlopeVariances[0]        = 0.233822f;
        _defaultSlopeVariances[1]        = 0.0204235f;
        _defaultSlopeVariances[2]        = 1.98253f;
        _defaultSlopeVariances[3]        = 378.261f;
        _defaultEdgeLengthVariances[0]   = 2.38493f;
        _defaultEdgeLengthVariances[1]   = 9.70185f;
        _defaultEdgeLengthVariances[2]   = 3.51697f;
        _defaultEdgeLengthVariances[3]   = 487.789f;
        _defaultEdgeScaleVariances[0]    = 0.00370166f;
        _defaultEdgeScaleVariances[1]    = 0.000938491f;
        _defaultEdgeScaleVariances[2]    = 0.118306f;
        _defaultEdgeScaleVariances[3]    = 0.532178f;
        _defaultEdgeStrengthVariances[0] = 1.22341e+006f;
        _defaultEdgeStrengthVariances[1] = 879251.0f;
        _defaultEdgeStrengthVariances[2] = 13.599f;
        _defaultEdgeStrengthVariances[3] = 3277.45f;

        _libraryElevationVariances[0]    = 8.43206e+008f;
        _libraryElevationVariances[1]    = 2.28953e+006f;
        _libraryElevationVariances[2]    = 18.6708f;
        _libraryElevationVariances[3]    = 191.095f;
        _librarySlopeVariances[0]        = 13.9547f;
        _librarySlopeVariances[1]        = 4.97565f;
        _librarySlopeVariances[2]        = 149.059f;
        _librarySlopeVariances[3]        = 17614.1f;
        _libraryEdgeLengthVariances[0]   = 241.721f;
        _libraryEdgeLengthVariances[1]   = 1196.83f;
        _libraryEdgeLengthVariances[2]   = 23.9746f;
        _libraryEdgeLengthVariances[3]   = 5460.74f;
        _libraryEdgeScaleVariances[0]    = 0.0803946f;
        _libraryEdgeScaleVariances[1]    = 0.0327203f;
        _libraryEdgeScaleVariances[2]    = 3.93968f;
        _libraryEdgeScaleVariances[3]    = 50.1937f;
        _libraryEdgeStrengthVariances[0] = 7.91633e+007f;
        _libraryEdgeStrengthVariances[1] = 8.03228e+007f;
        _libraryEdgeStrengthVariances[2] = 238.318f;
        _libraryEdgeStrengthVariances[3] = 105626.0f;
    
        break;
    }       

#endif
    
    // Done, and done!
    _analyzed = true;
}


void TLL::ensureLoaded() const {
    if (! loaded()) {
        for (IndexType i = 0; i < size(); ++i)
            terrainType(i).ensureLoaded();
        _loaded = true;
    }
}
void TLL::ensureAnalyzed() const {
    if (! analyzed()) {
//        ensureLoaded();
//        for (IndexType i = 0; i < size(); ++i)
//            terrainType(i).ensureAnalyzed();
        const_cast<TLL *>(this)->analyze();
    }
}
void TLL::ensureStudied() const {
    if (! studied()) {
        for (IndexType i = 0; i < size(); ++i)
            terrainType(i).ensureStudied();
        ensureAnalyzed();
        _studied = true;
    }
}


/*---------------------------------------------------------------------------*
 | Statistical distribution matching
 *---------------------------------------------------------------------------*/
const DM & TLL::elevationDistribution() const {
    ensureAnalyzed();
    return _elevationDistribution;
}
const DM & TLL::slopeDistribution() const {
    ensureAnalyzed();
    return _slopeDistribution;
}
const DM & TLL::edgeLengthDistribution() const {
    ensureAnalyzed();
    return _edgeLengthDistribution;
}
const DM & TLL::edgeScaleDistribution() const {
    ensureAnalyzed();
    return _edgeScaleDistribution;
}
const DM & TLL::edgeStrengthDistribution() const {
    ensureAnalyzed();
    return _edgeStrengthDistribution;
}

const VA & TLL::defaultElevationVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _defaultElevationVariances;
}
const VA & TLL::defaultSlopeVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _defaultSlopeVariances;
}
const VA & TLL::defaultEdgeLengthVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _defaultEdgeLengthVariances;
}
const VA & TLL::defaultEdgeScaleVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _defaultEdgeScaleVariances;
}
const VA & TLL::defaultEdgeStrengthVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _defaultEdgeStrengthVariances;
}

const VA & TLL::libraryElevationVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _libraryElevationVariances;
}
const VA & TLL::librarySlopeVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _librarySlopeVariances;
}
const VA & TLL::libraryEdgeLengthVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _libraryEdgeLengthVariances;
}
const VA & TLL::libraryEdgeScaleVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _libraryEdgeScaleVariances;
}
const VA & TLL::libraryEdgeStrengthVariances() const {
#if ANALYZE_MODE != 2
    ensureAnalyzed();
#endif
    return _libraryEdgeStrengthVariances;
}


/*---------------------------------------------------------------------------*
 | Access to parent TerrainLibrary's properties
 *---------------------------------------------------------------------------*/
SizeType TLL::size() const {
    return object().size();
}

TerrainSeamPtr TLL::terrainSeam(IndexType tt1,
                                IndexType tt2) {
    return object().terrainSeam(tt1, tt2);
}
TerrainSeamConstPtr TLL::terrainSeam(IndexType tt1,
                                     IndexType tt2) const {
    return object().terrainSeam(tt1, tt2);
}
TerrainSeamPtr TLL::terrainSeam(const std::string & tt1,
                                const std::string & tt2) {
    return object().terrainSeam(tt1, tt2);
}
TerrainSeamConstPtr TLL::terrainSeam(const std::string & tt1,
                                     const std::string & tt2) const {
    return object().terrainSeam(tt1, tt2);
}


/*****************************************************************************
 * TerrainLibrary containing all the TerrainType definitions
 *****************************************************************************/
/*---------------------------------------------------------------------------*
 | Constructors
 *---------------------------------------------------------------------------*/
// Default constructor with optional number of TerrainTypes
TL::TerrainLibrary(SizeType n) { }


/*---------------------------------------------------------------------------*
 | Accessor functions
 *---------------------------------------------------------------------------*/
// Number of TerrainTypes in the library
SizeType TL::size() const { return terrainTypes().size(); }


// Look up a TerrainType by name
IndexType TL::indexOf(const std::string & name) const {
    const TerrainTypeList & ttl = terrainTypes();
    TerrainTypeList::const_iterator it;
    for (it = ttl.begin(); it != ttl.end(); ++it)
        if ((*it)->name() == name)
            return it - ttl.begin();
    return -1;
}


// Access to the TerrainType objects
const TL::TerrainTypeList & TL::terrainTypes() const {
    // If this is the first time this was called, we need to do a litle extra
    // initialization -- creating a "Void" TerrainType representing empty
    // space. We can't do this in the constructor because the
    // shared_from_this call in addTerrainType isn't valid in the
    // constructor. Sigh.
    static bool initialized = false;
    if (! initialized) {
        initialized = true;     // Have to do this FIRST, 'cause we call recursively
        const_cast<TerrainLibrary*>(this)->addTerrainType("Void");
    }
    return _terrainTypes;
}
TerrainTypePtr TL::terrainType(IndexType i) {
    if (i < 0 || i >= IndexType(terrainTypes().size()))
        return TerrainTypePtr();
    else
        return _terrainTypes[i];
}
TerrainTypeConstPtr TL::terrainType(IndexType i) const {
    if (i < 0 || i >= IndexType(terrainTypes().size()))
        return TerrainTypeConstPtr();
    else
        return _terrainTypes[i];
}
TerrainTypePtr TL::terrainType(const std::string &tt) {
    return terrainType(indexOf(tt));
}
TerrainTypeConstPtr TL::terrainType(const std::string &tt) const {
    return terrainType(indexOf(tt));
}
TerrainTypePtr TL::randomTerrainType() {
    RandomUniform<IndexType> randomIndex(0, terrainTypes().size() - 1);
    return _terrainTypes[randomIndex()];
}
TerrainTypeConstPtr TL::randomTerrainType() const {
    RandomUniform<IndexType> randomIndex(0, terrainTypes().size() - 1);
    return _terrainTypes[randomIndex()];
}


// Access to the TerrainSeam objects
const TL::TerrainSeamMatrix & TL::terrainSeams() const {
    return _terrainSeams;
}
TerrainSeamPtr TL::terrainSeam(IndexType tt1, IndexType tt2) {
    // We must make sure that the first index is the greater of the two,
    // since this is how the TerrainSeams are stored in the triangular
    // matrix described above.
    IndexType ttMajor = std::max(tt1, tt2),
              ttMinor = std::min(tt1, tt2);
    if (ttMinor < 0 || ttMajor >= IndexType(terrainTypes().size()))
        return TerrainSeamPtr();
    else
        return _terrainSeams[ttMajor][ttMinor];
}
TerrainSeamConstPtr TL::terrainSeam(IndexType tt1, IndexType tt2) const {
    // See previous comment
    IndexType ttMajor = std::max(tt1, tt2),
              ttMinor = std::min(tt1, tt2);
    if (ttMinor < 0 || ttMajor >= IndexType(terrainTypes().size()))
        return TerrainSeamConstPtr();
    else
        return _terrainSeams[ttMajor][ttMinor];
}
TerrainSeamPtr TL::terrainSeam(const std::string &tt1,
                               const std::string &tt2) {
    return terrainSeam(indexOf(tt1), indexOf(tt2));
}
TerrainSeamConstPtr TL::terrainSeam(const std::string &tt1,
                                    const std::string &tt2) const {
    return terrainSeam(indexOf(tt1), indexOf(tt2));
}
TerrainSeamPtr TL::terrainSeam(TerrainTypeConstPtr tt1,
                               TerrainTypeConstPtr tt2) {
    IDType id1 = (tt1 == NULL ? 0 : tt1->terrainTypeID());
    IDType id2 = (tt2 == NULL ? 0 : tt2->terrainTypeID());
    return terrainSeam(id1, id2);
}
TerrainSeamConstPtr TL::terrainSeam(TerrainTypeConstPtr tt1,
                                    TerrainTypeConstPtr tt2) const {
    IDType id1 = (tt1 == NULL ? 0 : tt1->terrainTypeID());
    IDType id2 = (tt2 == NULL ? 0 : tt2->terrainTypeID());
    return terrainSeam(id1, id2);
}


/*---------------------------------------------------------------------------*
 | Library modification functions
 *---------------------------------------------------------------------------*/
TerrainTypePtr TL::addTerrainType(const std::string & name) {
    INCA_INFO("Adding terrain type " << name)

    TerrainLibraryPtr self = shared_from_this();

    // Create a new TerrainType object...
    TerrainTypePtr tt(new TerrainType(self, _terrainTypes.size(), name));
    _terrainTypes.push_back(tt);

    // ...then create a TerrainSeam for each new combination.
    _terrainSeams.resize(_terrainSeams.size() + 1);     // Expand to include one more vector
    for (IndexType i = 0; i < IndexType(_terrainTypes.size()); ++i) {// Fill with TSTs
        TerrainSeamPtr ts(new TerrainSeam(self, _terrainTypes.size() - 1, i));
        _terrainSeams.back().push_back(ts);
    }

    // Finally, put NULL in for the diagonal (no seam between identical TTs)
    _terrainSeams.back().push_back(TerrainSeamPtr());

    // Finally, pass along the newly created TT
    return tt;
}

TerrainTypePtr TL::addTerrainType(TerrainTypePtr tt) {
    INCA_ERROR("addTerrainType(TTP) not implemented...not sure why...")
    return TerrainTypePtr();
    // FIXME: this is broken! -- copy c_tor for props, and appropriate add/name/index
//    add();                  // Create a new TerrainType and its TerrainTypeSeams
//    *_terrainTypes.back() = *tt;    // Copy the attributes from 'tt'
}
