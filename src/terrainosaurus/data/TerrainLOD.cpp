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
#include <inca/raster/operators/arithmetic>
using namespace inca::math;
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
         if (effectivelyEqual(scalar_t(10), mps))     return LOD_10m;
    else if (effectivelyEqual(scalar_t(30), mps))     return LOD_30m;
    else if (effectivelyEqual(scalar_t(90), mps))     return LOD_90m;
    else if (effectivelyEqual(scalar_t(270), mps))    return LOD_270m;
    else if (effectivelyEqual(scalar_t(810), mps))    return LOD_810m;
//    else if (effectivelyEqual(scalar_t(2430), mps))   return LOD_2430m;
    else {
        INCA_ERROR("LODForMetersPerSample(): No LOD has a resolution of " << mps)
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

scalar_t terrainosaurus::blendFalloffRatio(TerrainLOD lod) {
    return scalar_t(0.4f);
}

scalar_t terrainosaurus::blendFalloffRadius(TerrainLOD lod) {
    return windowSize(lod) * blendFalloffRatio(lod) * scalar_t(0.5);
}

scalar_t terrainosaurus::blendOverlapRatio(TerrainLOD lod) {
    return scalar_t(0.25f);
}

DifferenceType terrainosaurus::blendPatchSpacing(TerrainLOD lod) {
    return DifferenceType(windowSize(lod) * (1 - blendOverlapRatio(lod)));
}

//#include <inca/raster/operators/statistic>

// Pixel blending masks
const GrayscaleImage & terrainosaurus::gaussianMask(TerrainLOD lod) {
    // Only create these the first time
    static bool initialized = false;
    static std::vector<GrayscaleImage> masks(TerrainLOD::maximum() + 1);
    if (! initialized) {
        typedef inca::Array<scalar_t, 2> ScalarArray;
        for (TerrainLOD tl = TerrainLOD::minimum(); tl <= TerrainLOD::maximum(); ++tl) {
            SizeType winSize = windowSize(tl);
            masks[tl] = select(gaussian(ScalarArray(blendFalloffRadius(lod)),
                                        ScalarArray(scalar_t(winSize) / 2 - 1)),
                               inca::Array<SizeType, 2>(winSize));
            masks[tl] /= masks[tl](winSize / 2, winSize / 2);
        }
    }

    return masks[lod];
}
const GrayscaleImage & terrainosaurus::sphericalMask(TerrainLOD lod) {
    // Only create these the first time
    static bool initialized = false;
    static std::vector<GrayscaleImage> masks(TerrainLOD::maximum() + 1);
    if (! initialized) {
        typedef inca::Array<scalar_t, 2> ScalarArray;
        for (TerrainLOD tl = TerrainLOD::minimum(); tl <= TerrainLOD::maximum(); ++tl) {
            GrayscaleImage & m = masks[tl];
            SizeType winSize = windowSize(tl);
            m.setSizes(winSize, winSize);
            scalar_t radius = scalar_t(winSize) / 2;
            Pixel px;
            for (px[0] = 0; px[0] < winSize; ++px[0])
                for (px[1] = 0; px[1] < winSize; ++px[1]) {
                    Dimension d(winSize / 2 - px[0], winSize / 2 - px[1]);
                    m(px) = std::max(scalar_t(0),
                                     scalar_t(std::sqrt(radius*radius - d[0]*d[0] - d[1]*d[1])) / radius);
                }
        }
    }

    return masks[lod];
}
const GrayscaleImage & terrainosaurus::coneMask(TerrainLOD lod) {
    static GrayscaleImage img;
    return img;
}
const GrayscaleImage & terrainosaurus::exponentialMask(TerrainLOD lod) {
    static GrayscaleImage img;
    return img;
}
