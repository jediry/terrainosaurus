// Import class definition
#include "HackWindow.hpp"
using namespace terrainosaurus;

#include <terrainosaurus/rendering/CameraProjection.hpp>
#include <terrainosaurus/MapExplorer.hpp>

#include <inca/util/Timer>

#define H_LOOK_SPEED    0.02f
#define V_LOOK_SPEED    0.005f
#define H_SPEED         50.0f
#define V_SPEED         50.0f
#define MAX_PHI         2.5f

#define GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>
#include <inca/ui.hpp>

// Constructor taking an image
HackWindow::HackWindow(TerrainSampleConstPtr ts, MapRasterizationConstPtr mr,
                       TerrainLOD startLOD, const std::string & title)
            : WINDOW(GUI_TOOLKIT)(title), lod(startLOD) {
    load(ts, mr);

    // Black background, auto-cleared
    renderer.rasterizer().setBackgroundColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
    renderer.addAutoClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer.rasterizer().setDepthBufferingEnabled(true);
    std::cerr << "Z-buffer? " << renderer.rasterizer().isDepthBufferingEnabled() << endl;

    // Enable lighting and shading
    Renderer::LightingUnit & light0 = renderer.lightingUnit(0);
    light0.setPosition(Point3D(1000.0f, 1000.0f, 1000.0f));
    light0.setDiffuseColor(Color(0.6f, 0.6f, 0.6f, 1.0f));
    light0.setEnabled(true);

    Renderer::LightingUnit & light1 = renderer.lightingUnit(1);
    light1.setPosition(Point3D(-1000.0f, 0.0f, 1000.0f));
    light1.setDiffuseColor(Color(0.2f, 0.2f, 0.2f, 1.0f));
    light1.setEnabled(true);

    GL::glutSetCursor(GLUT_CURSOR_NONE);
    setFullScreen(true);

    // Set up the camera
    camera.transform()->position = Point3D(500.0f, 500.0f, 300.0f);
    camera.nearClip = 50.0f;
    camera.farClip  = 100000.0f;
    camera.transform()->lookAt(Point3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f));
}

// Setter for a real, 2D image
void HackWindow::load(TerrainSampleConstPtr ts, MapRasterizationConstPtr mr) {
    _terrainSample = ts;
    _mapRasterization = mr;
    setLOD(lod);
}

void HackWindow::setLOD(TerrainLOD _lod) {
    if (_lod >= TerrainLOD::minimum() && _lod <= TerrainLOD::maximum()) {
        lod = _lod;
        renderer.rasterizer().setVertexArray(inca::rendering::DataArray<Nothing>());
        renderer.rasterizer().setColorArray(inca::rendering::DataArray<Nothing>());
        renderer.rasterizer().setNormalArray(inca::rendering::DataArray<Nothing>());
        renderHF.load((*_terrainSample)[lod], (*_mapRasterization)[lod]);
        requestRedisplay();
    }
}

void HackWindow::mouseMotion(int x, int y) {
    float dx = float(x) / getSize()[0],
          dy = 1.0f - float(y) / getSize()[1];
    float scale = dx + dy;
    camera.zoom(scale / 0.5f);
}

void HackWindow::passiveMotion(int x, int y) {
    Pixel center(getSize()[0] / 2);
    Dimension dx = Pixel(x, y) - center;
    dx[0] = -dx[0];
    int change = dot(dx, dx);
    if (change > 100)
        GL::glutWarpPointer(center[0], center[1]);
    else if (change > 0) {
//        cerr << "Change is " << change << " for dx " << dx;
        static const Vector3D X(1.0f, 0.0f, 0.0f);
        static const Vector3D Y(0.0f, 1.0f, 0.0f);
        static const Vector3D Z(0.0f, 0.0f, 1.0f);

        Vector3D front = camera.transform->front();
        Vector3D left = camera.transform()->left();
        Vector3D up = camera.transform->up();
        scalar_t theta = canonicalAngle(X, normalize(project(front, Z)), Z);
        scalar_t phi = signedAngle(up, Z, left);
        theta += dx[0] * H_LOOK_SPEED;
        phi   += dx[1] * V_LOOK_SPEED;
        if (phi > MAX_PHI)     phi = MAX_PHI;
        if (phi < -MAX_PHI)    phi = -MAX_PHI;
//        cerr << "   d T/P are " << (dx[0] * H_LOOK_SPEED) << " / " << (dx[1] * V_LOOK_SPEED);
//        cerr << "   new theta/phi are " << theta << " / " << phi << endl;
        scalar_t cos_p = cos(phi);
        Vector3D newFront(cos(theta) * cos_p, sin(theta) * cos_p, sin(phi));
        Vector3D newRight = newFront % Z;
        camera.transform->constructBasis(newFront, newRight);
        GL::glutWarpPointer(center[0], center[1]);
        requestRedisplay();
    }
}

void HackWindow::key(unsigned char k, int x, int y) {
    Vector3D direction;
    switch (k) {
    case 27:
        inca::ui::Application::instance().exit(0, "User pressed ESC");
        break;
    case 'a':
        direction = camera.transform->left();
        direction[2] = 0.0f;
        camera.transform->translate(normalize(direction) * H_SPEED);
        requestRedisplay();
        break;
    case 'd':
        direction = camera.transform->right();
        direction[2] = 0.0f;
        camera.transform->translate(normalize(direction) * H_SPEED);
        requestRedisplay();
        break;
    case 's':
        direction = camera.transform->back();
        direction[2] = 0.0f;
        camera.transform->translate(normalize(direction) * H_SPEED);
        requestRedisplay();
        break;
    case 'w':
        direction = camera.transform->front();
        direction[2] = 0.0f;
        camera.transform->translate(normalize(direction) * H_SPEED);
        requestRedisplay();
        break;
    case 'e':
        camera.transform->translateZ(V_SPEED);
        requestRedisplay();
        break;
    case 'q':
        camera.transform->translateZ(-V_SPEED);
        requestRedisplay();
        break;
    case 'f':
        if (isFullScreen())
            setFullScreen(false);
        else
            setFullScreen(true);
        break;
    }
}

void HackWindow::special(int k, int x, int y) {
    switch (k) {
    case GLUT_KEY_UP:
        setLOD(lod + 1);
        requestRedisplay();
        break;
    case GLUT_KEY_DOWN:
        setLOD(lod - 1);
        requestRedisplay();
        break;
    case GLUT_KEY_F1:
        renderHF.toggle("Points");
        requestRedisplay();
        break;
    case GLUT_KEY_F2:
        renderHF.toggle("Wireframe");
        requestRedisplay();
        break;
    case GLUT_KEY_F3:
        renderHF.toggle("Polygons");
        requestRedisplay();
        break;
    case GLUT_KEY_F4:
        renderHF.toggle("Lighting");
        requestRedisplay();
        break;
    }
}

void HackWindow::reshape(int width, int height) {
    GLUTWindow::reshape(width, height);
    camera.reshape(width, height);
    CameraProjection()(renderer, camera);
}

void HackWindow::display() {
    renderer.beginFrame();

//            cerr << "View from " << camera.transform->position() << " towards " << camera.transform->front() << endl;
        Renderer::MatrixStack & viewMat = renderer.viewMatrix(),
                                & projMat = renderer.projectionMatrix();
        viewMat.reset();
        viewMat.untransform(*camera.transform());
        renderHF(renderer);

    renderer.endFrame();
    GL::glutSwapBuffers();
}

// Rotate by whatever amount
void HackWindow::idle() {
/*    static ::inca::Timer<scalar_t, false> timer;

    timer.stop();
    camera.transform->orbit(_orbitVelocity * timer(), Vector3D(0.0f, 0.0f, 1.0f), Point3D(0.0f));
    timer.reset();
    timer.start();
    requestRedisplay();*/
}
