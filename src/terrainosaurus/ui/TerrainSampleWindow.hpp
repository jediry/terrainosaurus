/** -*- C++ -*-
 *
 * \file    TerrainSampleWindow.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The TerrainSampleWindow class is a top-level Window subclass for
 *      displaying a TerrainSample in 3D and for monitoring the progress of the
 *      heightfield refinement algorithm.
 *
 *      FIXME: the determination of GA/not is kinda ghetto
 *              TerrainSample is overloaded in this context
 */

#pragma once
#ifndef TERRAINOAURUS_UI_TERRAIN_SAMPLE_WINDOW
#define TERRAINOAURUS_UI_TERRAIN_SAMPLE_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainSampleWindow;

    // Pointer typedefs
    typedef shared_ptr<TerrainSampleWindow>       TerrainSampleWindowPtr;
    typedef shared_ptr<TerrainSampleWindow const> TerrainSampleWindowConstPtr;
}

// Import superclass definition
#include <inca/ui/Window.hpp>

// Import data objects
#include <terrainosaurus/data/TerrainSample.hpp>


class terrainosaurus::TerrainSampleWindow : public inca::ui::Window {
public:
    // Constructor taking a view-only TerrainSample
    explicit TerrainSampleWindow(TerrainSamplePtr ts,
                                 const std::string & title = "Terrain Viewer");

    // Constructor taking a pattern TerrainSample and a to-build TerrainSample
    explicit TerrainSampleWindow(TerrainSamplePtr ts, TerrainSamplePtr ps,
                                 const std::string & title = "Terrain Generator");

    // TerrainSample accessors
    TerrainSamplePtr terrainSample();
    void setTerrainSample(TerrainSamplePtr ts);

    // Pattern TerrainSample accessors
    TerrainSamplePtr patternSample();
    void setPatternSample(TerrainSamplePtr ps);

protected:
    // UI construction function
    void construct();

    TerrainSamplePtr    _terrainSample;
    TerrainSamplePtr    _patternSample;
};

#endif
