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
MapEditorWindow::MapEditorWindow(MapPtr m, const std::string & title)
            : WINDOW(GUI_TOOLKIT)(title) {

    MeshSelectionPtr ps(new MeshSelection());
    MeshSelectionPtr ts(new MeshSelection());
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

    this->setSize(Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
    if (FULL_SCREEN)    this->setFullScreen(true);
    else                this->centerOnScreen();
}

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
