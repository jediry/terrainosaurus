#ifndef TERRAINOAURUS_UI_HACK_WINDOW
#define TERRAINOAURUS_UI_HACK_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class HackWindow;
}

// Import superclass definition
#include <inca/ui.hpp>

#include <terrainosaurus/rendering/HeightfieldRendering.hpp>


// Simple window displaying an image
class terrainosaurus::HackWindow : public WINDOW(GUI_TOOLKIT) {
public:
    // Constructor taking an image
    HackWindow(TerrainSampleConstPtr ts, MapRasterizationConstPtr mr,
               TerrainLOD startLOD, const std::string & title = "Terrain Sample");

    // Setter for a real, 2D image
    void load(TerrainSampleConstPtr ts, MapRasterizationConstPtr mr);
    void setLOD(TerrainLOD lod);

    // event handlers
    void mouseMotion(int x, int y);
    void passiveMotion(int x, int y);
    void key(unsigned char k, int x, int y);
    void special(int k, int x, int y);
    void reshape(int width, int height);
    void display();
    void idle();

protected:
    TerrainSampleConstPtr       _terrainSample;
    MapRasterizationConstPtr    _mapRasterization;
    TerrainLOD lod;

    HeightfieldRendering            renderHF;
    inca::world::PerspectiveCamera  camera;
    Renderer                        renderer;
};

#endif
