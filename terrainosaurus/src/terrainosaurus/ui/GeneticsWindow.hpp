#ifndef TERRAINOAURUS_UI_GENETICS_WINDOW
#define TERRAINOAURUS_UI_GENETICS_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class GeneticsWindow;
}

// Import superclass definition
#include <inca/ui.hpp>

// Import rendering object definitions
#include "ObjectRenderingView.hpp"
#include <terrainosaurus/rendering/ChromosomeRendering.hpp>


class terrainosaurus::GeneticsWindow : public WINDOW(GUI_TOOLKIT) {
public:
    // HACK
    typedef ObjectRenderingView<ChromosomeRendering>    RenderingView;
    typedef shared_ptr<RenderingView>                   RenderingViewPtr;

    // Constructor taking an image
    GeneticsWindow(TerrainChromosome & c,
                   const std::string & title = "Genetics Sandbox");

    // Chromosome accessor functions
    TerrainChromosome & chromosome();
    void setChromosome(TerrainChromosome & c);

    // Event-handling functions
    void display();
    void key(unsigned char k, int x, int y);
    void special(int k, int x , int y);

protected:
    TerrainChromosome *     _chromosome;
    RenderingViewPtr        renderView;
};

#endif
