/*
 * File: DEMTest.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Description:
 *      This program tests the functionality of the DEMInterpreter.
 */


/*****************************************************************************
 * Window/layout parameters
 *****************************************************************************/
// GUI layout parameters
const char * WINDOW_TITLE = "DEM Tester";
const int WINDOW_WIDTH  = 640;
const int WINDOW_HEIGHT = 480;

// Import DEM reader
#include "DEMInterpreter.hpp"
#include "ElevationMap.hpp"
using namespace Terrainosaurus;


class SceneViewWidget : public CameraControl, public SceneView, public Widget {
public:
    SceneViewWidget(ScenePtr s, CameraPtr c)
        : CameraControl(c), SceneView(s, c) { }

    void renderView() {
        SceneView::renderView();
        cerr << "Rendered " << renderer.triangleCount() << " triangles\n";
    }

/*    void mouseDragged(unsigned int x, unsigned int y) {
        int dx = x - mouseX;
        int dy = mouseY - y;

        lookCamera(dx, dy);

        mouseX = x;
        mouseY = y;
    }*/
    
    void buttonPressed(MouseButton button,
                       unsigned int x, unsigned int y) {
        mouseX = x;
        mouseY = y;
    }

    void keyPressed(KeyCode key, unsigned int x, unsigned int y) {
        switch (key) {
            case KEY_LEFT:
                panCamera(1, 0);
                break;
            case KEY_RIGHT:
                panCamera(-1, 0);
                break;
            case KEY_UP:
                panCamera(0, -1);
                break;
            case KEY_DOWN:
                panCamera(0, 1);
                break;
            case KEY_LESS_THAN:
                yawCamera(-1);
                break;
            case KEY_GREATER_THAN:
                yawCamera(1);
                break;
            case KEY_A:
                dollyCamera(1);
                break;
            case KEY_Z:
                dollyCamera(-1);
                break;
            case KEY_SPACE: case KEY_ENTER:
                if (! application->timer().isRunning())
                    application->timer().start();
                else
                    application->timer().stop();
                break;
            case KEY_Q: case KEY_ESCAPE: case KEY_F1:
                application->exit(0, "Exited normally");
            default:
                CameraControl::keyPressed(key, x, y);
                break;
        }
    }
};


// Application class definition
namespace Terrainosaurus {
    class DEMTest : public GLUTApplication {
    public:
        void setup(int &argc, char **argv) {
            scene = ScenePtr(new Scene());

            camera = CameraPtr(new PerspectiveCamera());
            camera->transform->setLocationPoint(new Transform::Point(200, 200, 200));
//            camera->transform->setLocationPoint(new Transform::Point(0.0, 0.0, 50.0));
            camera->transform->lookAt(Transform::Point(0.0), Transform::Vector(0.0, 1.0, 0.0));
            camera->nearClippingDistance = 10.0;
            camera->farClippingDistance = 10000.0;

            // Load the file
            map = ElevationMapPtr(new ElevationMap());
            DEMInterpreter dem(map);
            dem.filename = string("data\\eagle30.dem");
            dem.parse();
            map->transform->scale(Transform::Vector(1.0, 0.1, 1.0));
            map->transform->translate(Transform::Vector(0.0, -310, 0.0));
            scene->addObject(map);

            LightPtr lite(new Light());
            lite->intensity = 1.0;
            lite->transform->setLocationPoint(new Transform::Point(0.0, 30.0, -30.0));
            scene->addLight(lite);
        }

        // Function required by GLUTApplication
        void constructInterface() {
            window = new GLUTWidgetContainer(WINDOW_TITLE);
            window->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
            window->setWidget(new SceneViewWidget(scene, camera));
        }

    protected:
        GLUTWidgetContainer *window;
        ScenePtr scene;
        CameraPtr camera;
        ElevationMapPtr map;
    };
};


/*****************************************************************************
 * GLUTMapEditor main() entry function -- this creates the application and
 * launches it.
 *****************************************************************************/
int main(int argc, char **argv) {
    Terrainosaurus::DEMTest app;
    app.initialize(argc, argv);
    return app.run();
}
