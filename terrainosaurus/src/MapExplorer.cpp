/*
 * File: MapExplorer.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Description:
 */


// GUI parameters
const char * WINDOW_TITLE = "Terrainosaurus Map Editor";
const int WINDOW_WIDTH  = 640;
const int WINDOW_HEIGHT = 480;


// Import library configuration
#include "terrainosaurus-common.h"

// Import superclass definition
#include <inca/ui.hpp>
using namespace inca::ui;

// Import the Widget definition
#include "MapExplorerWidget.hpp"

// Choose underlying GUI toolkit
#define DEFAULT_TOOLKIT GLUT


// Application class definition
namespace terrainosaurus {
    class MapExplorer : public APPLICATION(DEFAULT_TOOLKIT) {
    public:
        // Put together our user interface
        void constructInterface() {
            // First, let's make the Widget that does everything
            explorer = WidgetPtr(new MapExplorerWidget("Map Explorer"));

            // Now, makea window to hold it
            window = createWindow(explorer, WINDOW_TITLE);
            window->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
            window->centerOnScreen();
        }

    protected:
        WindowPtr window;
        WidgetPtr explorer;
    };
};

// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::MapExplorer);

