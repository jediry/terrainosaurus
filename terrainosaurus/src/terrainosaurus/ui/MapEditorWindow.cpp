// Import class definition
#include "MapEditorWindow.hpp"
using namespace terrainosaurus;

#include <terrainosaurus/MapExplorer.hpp>

// Import widget definitions
//#include "ui/MapExplorerWidget.hpp"
//#include "ui/CameraNavigationWidget.hpp"
//#include "ui/CameraFlyWidget.hpp"
//#include "ui/MapEditWidget.hpp"
//#include "ui/AddFaceWidget.hpp"
//#include "ui/AddVertexWidget.hpp"
//#include "ui/ModifyPropertyWidget.hpp"
//#include "ui/TranslateWidget.hpp"
//#include "ui/RotateWidget.hpp"
//#include "ui/ScaleWidget.hpp"

#include <inca/util/Timer>

#define H_LOOK_SPEED    0.02f
#define V_LOOK_SPEED    0.005f
#define H_SPEED         50.0f
#define V_SPEED         50.0f
#define MAX_PHI         2.5f

#define WINDOW_WIDTH    400
#define WINDOW_HEIGHT   400
#define FULL_SCREEN     false

#define GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>
#include <inca/ui.hpp>


// Constructor
MapEditorWindow::MapEditorWindow(MapPtr m, MeshSelectionPtr ps, MeshSelectionPtr ts,
                                 const std::string & title)
            : WINDOW(GUI_TOOLKIT)(title) {
    load(m, ps, ts);

    // Black background, auto-cleared
    renderer.rasterizer().setBackgroundColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
    renderer.addAutoClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer.rasterizer().setDepthBufferingEnabled(false);
//     renderer.rasterizer().setLineSmoothingEnabled(true);
//     renderer.rasterizer().setPointSmoothingEnabled(true);
//     renderer.rasterizer().setAlphaBlendingEnabled(true);

    // Set up the camera
    camera.transform()->position = Point3D(0.0f, 0.0f, 100.0f);
    camera.viewWidth    = 15.0;
    camera.viewHeight   = 15.0;
    camera.nearClip     = 50.0f;
    camera.farClip      = 1000.0f;
    camera.transform()->lookAt(Point3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));

#if 0
    ///////////////////////////////////////////////////////////////////////////
    // Build the widget stack

    // The MapExplorerWidget handles application events, draws the UI and
    // chooses between major application modes (editing, walk-thru, etc.)
    MapExplorerWidgetPtr explorer(new MapExplorerWidget("Map Explorer"));

    // The 2D map editing mode widget stack
    OrthographicCameraPtr camera2D(new OrthographicCamera());
        camera2D->viewWidth = 15.0;
        camera2D->viewHeight = 15.0;
        camera2D->nearClip = -1.0;
        camera2D->transform->position = Transform::Point(0.0, 0.0, 100.0);
    CameraNavigationWidgetPtr navigate2D(new CameraNavigationWidget("2D Navigation"));
    MultiplexorWidgetPtr toolSelect2D(new MultiplexorWidget("2D Tool Select"));
    MapEditWidgetPtr modifyProperty2D(new ModifyPropertyWidget("Modify Property"));
    MapEditWidgetPtr addFace2D   (new AddFaceWidget("Add Face"));
    MapEditWidgetPtr addVertex2D (new AddVertexWidget("Add Vertex"));
    MapEditWidgetPtr translate2D (new TranslateWidget("Translate"));
    MapEditWidgetPtr rotate2D    (new RotateWidget("Rotate"));
    MapEditWidgetPtr scale2D     (new ScaleWidget("Scale"));
        explorer->addWidget(navigate2D);
        explorer->toolSelect2D = toolSelect2D;
            navigate2D->widget = toolSelect2D;
            navigate2D->camera = camera2D;  // Control the 2D camera
            navigate2D->enableLook = false;     // Disable capabilities
            navigate2D->enableDolly = false;    // that don't make sense
            navigate2D->enableYaw = false;      // in two dimensions
            navigate2D->enablePitch = false;
            navigate2D->rollScale = 3.1415962 / 150.0;  // Tweak the controls
            navigate2D->zoomScale = 1.001;
                toolSelect2D->addWidget(modifyProperty2D);
                toolSelect2D->addWidget(addFace2D);
                toolSelect2D->addWidget(addVertex2D);
 //               toolSelect2D->addWidget(translate2D);
 //               toolSelect2D->addWidget(rotate2D);
 //               toolSelect2D->addWidget(scale2D);


    // The 3D fly-through mode widget stack
    PerspectiveCameraPtr cameraFly3D(new PerspectiveCamera());
        cameraFly3D->horizViewAngle = PI<Camera::scalar_t>() / 3.0;
        cameraFly3D->vertViewAngle = PI<Camera::scalar_t>() / 3.0;
        cameraFly3D->nearClip = 1.0;
        cameraFly3D->transform->position = Transform::Point(2.0, 2.0, 2.0);
        cameraFly3D->transform->lookAt(Transform::Point(0.0, 0.0, 0.0));
    CameraFlyWidgetPtr fly3D(new CameraFlyWidget("2D Flythru"));
    MapEditWidgetPtr edit3D(new MapEditWidget("3D Edit"));
        explorer->addWidget(fly3D);
            fly3D->widget = edit3D;
            fly3D->camera = cameraFly3D;


    // XXX Pass the renderer to everyone. This is ugly
    TerrainosaurusRendererPtr renderer(new TerrainosaurusRenderer());
    explorer->renderer = renderer;
    navigate2D->renderer = renderer;
    modifyProperty2D->renderer = renderer;
    addFace2D->renderer = renderer;
    addVertex2D->renderer = renderer;
    translate2D->renderer = renderer;
    rotate2D->renderer = renderer;
    scale2D->renderer = renderer;

    fly3D->renderer = renderer;
    edit3D->renderer = renderer;


    // Now, make a window to hold it all
    window = createWindow(explorer, WINDOW_TITLE);
    window->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (FULL_SCREEN)    window->setFullScreen(true);
    else                window->centerOnScreen();
#endif
    this->setSize(Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
    if (FULL_SCREEN)    this->setFullScreen(true);
    else                this->centerOnScreen();
}

// Setter for a real, 2D image
void MapEditorWindow::load(MapPtr m, MeshSelectionPtr ps, MeshSelectionPtr ts) {
    _map = m;
    _persistentSelection = ps;
    _transientSelection = ts;
    renderMap.setMap(m);
    renderMap.setPersistentSelection(ps);
    renderMap.setTransientSelection(ts);
}

void MapEditorWindow::mouseMotion(int x, int y) {

}

void MapEditorWindow::passiveMotion(int x, int y) {

}

void MapEditorWindow::key(unsigned char k, int x, int y) {
    switch (k) {
        case 'e':
            renderMap.toggle("Envelopes");
            requestRedisplay();
            break;
        case 't':
            renderMap.toggle("Tangents");
            requestRedisplay();
            break;
        case 'r':
            renderMap.toggle("Refinements");
            requestRedisplay();
            break;
        case 'V':
            renderMap.toggle("Vertices");
            requestRedisplay();
            break;
        case 'E':
            renderMap.toggle("Edges");
            requestRedisplay();
            break;
        case 'F':
            renderMap.toggle("Faces");
            requestRedisplay();
            break;
        case ' ':
            _map->refine();
            requestRedisplay();
            break;
    }
}

void MapEditorWindow::special(int k, int x, int y) {
    static float newSmooth = 0.2f;
    switch (k) {
        case GLUT_KEY_UP:
            newSmooth += 0.1f;
            if (newSmooth > 1.0f) newSmooth = 1.0f;
            for (int i = 0; i < _map->terrainLibrary()->size(); ++i)
                for (int j = i + 1; j < _map->terrainLibrary()->size(); ++j)
                    _map->terrainLibrary()->terrainSeam(i, j)->smoothness = newSmooth;
            _map->refine();
            requestRedisplay();
            break;
        case GLUT_KEY_DOWN:
            newSmooth -= 0.1f;
            if (newSmooth < 0.0f) newSmooth = 0.0f;
            for (int i = 0; i < _map->terrainLibrary()->size(); ++i)
                for (int j = i + 1; j < _map->terrainLibrary()->size(); ++j)
                    _map->terrainLibrary()->terrainSeam(i, j)->smoothness = newSmooth;
            _map->refine();
            requestRedisplay();
            break;
    }
}

void MapEditorWindow::reshape(int width, int height) {
    GLUTWindow::reshape(width, height);
    camera.reshape(width, height);
//    CameraProjection()(renderer, camera);
//    cerr << "My proj mat \n" << renderer.projectionMatrix().get() << endl;
    renderer.projectionMatrix().reset();
    GL::glOrtho(-camera.viewWidth() / 2, camera.viewWidth() / 2,
                -camera.viewHeight() / 2, camera.viewHeight () / 2, camera.nearClip(), camera.farClip());
//    cerr << "GL's proj mat\n" << renderer.projectionMatrix().get() << endl;
}

void MapEditorWindow::display() {
    renderer.beginFrame();

        Renderer::MatrixStack & viewMat = renderer.viewMatrix(),
                              & projMat = renderer.projectionMatrix();
        viewMat.reset();
        viewMat.untransform(*camera.transform());
        renderMap(renderer);
//        GL::glutSolidSphere(10.0f, 5, 5);

    renderer.endFrame();
    GL::glutSwapBuffers();
}

// Rotate by whatever amount
void MapEditorWindow::idle() {

}
