/*
 * File: TerrainLOD.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import function prototypes
#include "TerrainLOD.hpp"
using namespace terrainosaurus;

// Import raster operators
#include <inca/raster/generators/gaussian>
#include <inca/raster/operators/select>
using namespace inca::raster;


// Spatial resolution for a terrain LOD
scalar_t terrainosaurus::metersPerSampleForLOD(TerrainLOD lod) {
    switch (lod) {
        case LOD_10m:       return scalar_t(10);
        case LOD_30m:       return scalar_t(30);
        case LOD_90m:       return scalar_t(90);
        case LOD_270m:      return scalar_t(270);
        case LOD_810m:      return scalar_t(810);
//        case LOD_2430m:     return scalar_t(2430);
        default:
            std::cerr << "metersPerSample(): Illegal LOD "
                      << int(lod) << std::endl;
            return scalar_t(1);
    }
}
scalar_t terrainosaurus::samplesPerMeterForLOD(TerrainLOD lod) {
    return scalar_t(1) / metersPerSampleForLOD(lod);
}
TerrainLOD terrainosaurus::LODForMetersPerSample(scalar_t mps) {
         if (math::effectivelyEqual(scalar_t(10), mps))     return LOD_10m;
    else if (math::effectivelyEqual(scalar_t(30), mps))     return LOD_30m;
    else if (math::effectivelyEqual(scalar_t(90), mps))     return LOD_90m;
    else if (math::effectivelyEqual(scalar_t(270), mps))    return LOD_270m;
    else if (math::effectivelyEqual(scalar_t(810), mps))    return LOD_810m;
//    else if (math::effectivelyEqual(scalar_t(2430), mps))   return LOD_2430m;
    else {
        std::cerr << "LODForMetersPerSample(): No LOD has a resolution of "
                  << mps << std::endl;
        return TerrainLOD_Overflow;
    }
}
TerrainLOD terrainosaurus::LODForSamplesPerMeter(scalar_t spm) {
    return LODForMetersPerSample(scalar_t(1) / spm);
}


// Scale factor for converting sizes at one LOD into another LOD
scalar_t terrainosaurus::scaleFactor(TerrainLOD from, TerrainLOD to) {
    return metersPerSampleForLOD(from) / metersPerSampleForLOD(to);
}


SizeType terrainosaurus::windowSize(TerrainLOD lod) {
    return 16;
}


// Pixel blending masks
const GrayscaleImage & terrainosaurus::gaussianMask(TerrainLOD lod) {
    // Only create these the first time
    static bool initialized = false;
    static std::vector<GrayscaleImage> masks(TerrainLOD::maximum() + 1);
    if (! initialized) {
        typedef inca::Array<scalar_t, 2> ScalarArray;
        for (TerrainLOD tl = TerrainLOD::minimum(); tl <= TerrainLOD::maximum(); ++tl) {
            SizeType sz = windowSize(tl);
            masks[tl] = select(gaussian(ScalarArray(scalar_t(sz) / 2),
                                        ScalarArray(scalar_t(sz) / 8)),
                               inca::Array<SizeType, 2>(sz));
        }
    }
    return masks[lod];
}
