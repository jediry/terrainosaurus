/**
 * \file HeightfieldWindow.hpp
 *
 * \author Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The HeightfieldWindow class is a top-level Window subclass for
 *      displaying heightfields in 3D and for monitoring the progress of a
 *      heightfield refinement algorithm.
 */

#ifndef TERRAINOAURUS_UI_HEIGHTFIELD_WINDOW
#define TERRAINOAURUS_UI_HEIGHTFIELD_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class HeightfieldWindow;

    // Pointer typedefs
    typedef shared_ptr<HeightfieldWindow>       HeightfieldWindowPtr;
    typedef shared_ptr<HeightfieldWindow const> HeightfieldWindowConstPtr;
}

// Import superclass definition
#include <inca/ui.hpp>

// Import data objects
#include <terrainosaurus/data/MapRasterization.hpp>
#include <terrainosaurus/data/TerrainSample.hpp>


class terrainosaurus::HeightfieldWindow
        : public WINDOW(GUI_TOOLKIT) {
public:
    // Constructor taking a TerrainSample
    explicit HeightfieldWindow(TerrainSamplePtr ts,
                               const std::string & title = "Heightfield Viewer");

    // Constructor taking a MapRasterization
    explicit HeightfieldWindow(MapRasterizationPtr mr,
                               const std::string & title = "Heightfield Viewer");

    // MapRasterization accessors
    MapRasterizationPtr mapRasterization();
    void setMapRasterization(MapRasterizationPtr mr);

    // TerrainSample accessors
    TerrainSamplePtr terrainSample();
    void setTerrainSample(TerrainSamplePtr ts);

protected:
    // UI construction function
    void construct();

    MapRasterizationPtr _mapRasterization;
    TerrainSamplePtr    _terrainSample;
};

#endif
