#ifndef TERRAINOAURUS_UI_MAP_EDITOR_WINDOW
#define TERRAINOAURUS_UI_MAP_EDITOR_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapEditorWindow;

    // Pointer typedefs
    typedef shared_ptr<MapEditorWindow>       MapEditorWindowPtr;
    typedef shared_ptr<MapEditorWindow const> MapEditorWindowConstPtr;
}

// Import superclass definition
#include <inca/ui.hpp>

// Import rendering object definitions
#include <terrainosaurus/rendering/MapRendering.hpp>
#include <terrainosaurus/rendering/GridRendering.hpp>


// Simple window displaying an image
class terrainosaurus::MapEditorWindow : public WINDOW(GUI_TOOLKIT) {
public:
    // Constructor taking a map object to edit
    MapEditorWindow(MapPtr m, const std::string & title = "Map Editor");
    MapEditorWindow(MapPtr m, MeshSelectionPtr ps, MeshSelectionPtr ts,
                    const std::string & title = "Map Editor");

    // Setter data objects
    void load(MapPtr m, MeshSelectionPtr ps, MeshSelectionPtr ts);

    // event handlers
    void mouseMotion(int x, int y);
    void passiveMotion(int x, int y);
    void key(unsigned char k, int x, int y);
    void special(int k, int x, int y);
    void reshape(int width, int height);
    void display();
    void idle();

protected:
    MapPtr              _map;
    MeshSelectionPtr    _persistentSelection,
                        _transientSelection;

    MapRendering                    renderMap;
    GridRendering                   renderGrid;
    inca::world::OrthographicCamera camera;
    Renderer                        renderer;
};

#endif
