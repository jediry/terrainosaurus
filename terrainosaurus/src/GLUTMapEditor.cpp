/*
 * File: GLUTMapEditor.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Description:
 */


/*****************************************************************************
 * Window/layout parameters
 *****************************************************************************/
// GUI layout parameters
const char * WINDOW_TITLE = "Terrainosaurus Map Editor";
const int WINDOW_WIDTH  = 640;
const int WINDOW_HEIGHT = 480;


// Import Inca's C++ wrapper around GLUT
#include <interface/glut/GLUTApplication.h++>
#include <interface/glut/GLUTWidgetContainer.h++>
using namespace Inca::Interface;

// Import the Widget definition
#include "MapEditorWidget.hpp"
using namespace Terrainosaurus;


// Application class definition
namespace Terrainosaurus {
    class GLUTMapEditor : public GLUTApplication {
    public:
        // Function required by GLUTApplication
        void constructInterface() {
            window = new GLUTWidgetContainer(WINDOW_TITLE);
            window->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
            window->setWidget(new MapEditorWidget());
    //        window->centerOnScreen();
    //        window->setVisible(true);
        }

    protected:
        GLUTWidgetContainer *window;
    };
};


/*****************************************************************************
 * GLUTMapEditor main() entry function -- this creates the application and
 * launches it.
 *****************************************************************************/
int main(int argc, char **argv) {
    Terrainosaurus::GLUTMapEditor app;
    app.initialize(argc, argv);
    return app.run();
}
