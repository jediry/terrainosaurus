#ifndef TERRAINOAURUS_UI_TERRAIN_SAMPLE_WINDOW
#define TERRAINOAURUS_UI_TERRAIN_SAMPLE_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

#include "ImageWindow.hpp"


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class TerrainSampleWindow;
}

// Import augmented enumeration mechanism
#include <inca/util/Enumeration.hpp>

// Import raster operators
#include <inca/raster/operators/magnitude>


// What thing do we want to see?
INCA_ENUM( SampleVisualizationMode,
         ( Elevation,
         ( GradientMagnitude,
         ( Edges,
         ( Ridges,
         ( ScaleSpace,
           BOOST_PP_NIL ) ) ) ) ) );


// Simple window displaying an image
class terrainosaurus::TerrainSampleWindow : public ImageWindow {
public:
    // Constructor taking an image
    TerrainSampleWindow(TerrainSampleConstPtr s,
                        MapRasterizationConstPtr m,
                        TerrainLOD startLOD,
                        const std::string &title = "Terrain Sample")
            : ImageWindow((*s)[startLOD].elevations(), title), lod(_lod) {
        load(s);
    }

    // Setter for a real, 2D image
    void load(TerrainSampleConstPtr s) {
        cerr << "Loading TerrainSample\n";
        sample = s;
    }

    void key(char k, int x, int y) {
        bool change = false;
        switch (k) {
            case 'a':
                vizMode--;
                if (vizMode == SampleVisualizationMode_Underflow)
                    vizMode = SampleVisualizationMode::maximum();
                change = true;
                break;
            case 'd':
                vizMode++;
                if (vizMode == SampleVisualizationMode_Overflow)
                    vizMode = SampleVisualizationMode::minimum();
                change = true;
                break;
            case 'w':
                lod++;
                if (lod == TerrainLOD_Overflow)
                    lod = TerrainLOD::minimum();
                change = true;
                break;
            case 's':
                lod--;
                if (lod == TerrainLOD_Underflow)
                    lod = TerrainLOD::maximum();
                change = true;
                break;
        }

        if (change) {
            switch (vizMode) {
                case Elevation:
                    cerr << "Displaying elevation image for " << lod << endl;
                    this->loadImage((*sample)[lod].elevations());
                    break;
                case GradientMagnitude:
                    cerr << "Displaying gradient image for " << lod << endl;
                    this->loadImage(magnitude((*sample)[lod].gradients()));
                    break;
                case Edges:
                    cerr << "Displaying edge image for " << lod << endl;
//                    this->loadImage(sample->edgeImage(lod));
                    break;
                case Ridges:
                    cerr << "Displaying ridge image for " << lod << endl;
//                    this->loadImage(sample->ridgeImage(lod));
                    break;
                case ScaleSpace:
                    cerr << "Displaying scalespace image for " << lod << endl;
//                    this->loadImage(sample->hyper);
                    break;
            }
            requestRedisplay();
        }
    }

protected:
    TerrainSampleConstPtr sample;
    SampleVisualizationMode vizMode;
    TerrainLOD lod;
};

#endif
