#ifndef TERRAINOAURUS_UI_GENETICS_WINDOW
#define TERRAINOAURUS_UI_GENETICS_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class GeneticsWindow;

    // Pointer typedefs
    typedef shared_ptr<GeneticsWindow>       GeneticsWindowPtr;
    typedef shared_ptr<GeneticsWindow const> GeneticsWindowConstPtr;
}

// Import superclass definition
#include <inca/ui.hpp>

// Import GA and data objects
#include <terrainosaurus/data/MapRasterization.hpp>
#include <terrainosaurus/genetics/HeightfieldGA.hpp>

// Import rendering object definitions
#include "ObjectRenderingView.hpp"
#include <terrainosaurus/rendering/ChromosomeRendering.hpp>


class terrainosaurus::GeneticsWindow
        : public WINDOW(GUI_TOOLKIT),
          public GeneticAlgorithmListener {
public:
    // HACK
    typedef ObjectRenderingView<ChromosomeRendering>    RenderingView;
    typedef shared_ptr<RenderingView>                   RenderingViewPtr;

    // Constructor taking an image
    GeneticsWindow(MapRasterizationPtr mr,
                   const std::string & title = "Genetics Sandbox");

    // MapRasterization accessors
    MapRasterizationPtr mapRasterization();
    void setMapRasterization(MapRasterizationPtr mr);

    // Selected chromosome controls
    SizeType chromosomeCount() const;
    IndexType selectedChromosomeIndex() const;
    void setSelectedChromosomeIndex(IndexType idx);
    void selectPreviousChromosome();
    void selectNextChromosome();

    // Visible LOD controls (also triggers GA to generate higher LODs)
    TerrainLOD selectedLOD() const;
    void setSelectedLOD(TerrainLOD lod);
    void selectPreviousLOD();
    void selectNextLOD();

    // GA-event-handling functions
//    void

    // Event-handling functions
    void display();
    void key(unsigned char k, int x, int y);
    void special(int k, int x , int y);

protected:
    HeightfieldGA       _heightfieldGA;
    IndexType           _selectedChromosomeIndex;
    TerrainLOD          _selectedLOD;

    // GUI/rendering objects
    RenderingViewPtr    _renderView;    // Render the selected chromosome
};

#endif
