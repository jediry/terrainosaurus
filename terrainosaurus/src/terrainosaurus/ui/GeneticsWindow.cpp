// Import class definition
#include "GeneticsWindow.hpp"

#define GL_HPP_IMPORT_GLUT 1
#include <inca/integration/opengl/GL.hpp>

#include <inca/ui/CameraNavigationWidget.hpp>

using namespace terrainosaurus;
using namespace inca::ui;
using namespace inca::world;

#define FULL_SCREEN false
#define WINDOW_WIDTH    400
#define WINDOW_HEIGHT   300


// Constructor taking an image
GeneticsWindow::GeneticsWindow(MapRasterizationPtr mr,
                               const std::string & title)
            : WINDOW(GUI_TOOLKIT)(title) {
    // Set up the window
    setSize(Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
    setFullScreen(FULL_SCREEN);
    GL::glutSetCursor(GLUT_CURSOR_NONE);

    // Set up the camera
    CameraPtr camera(new PerspectiveCamera());
    camera->transform()->position = Point3D(1000.0f, 1000.0f, 500.0f);
    camera->nearClip = 50.0f;
    camera->farClip  = 100000.0f;
    camera->transform()->lookAt(Point3D(0.0f, 0.0f, 550.0f), Vector3D(0.0f, 0.0f, 1.0f));
    camera->transform()->lookAt(Point3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 1.0f));

    // Set up the widget stack
    CameraNavigationWidgetPtr nav(new CameraNavigationWidget(camera));
        nav->longitudinalMotionScale = 30.0f;
        nav->lateralMotionScale      = 30.0f;
        nav->verticalMotionScale     = 30.0f;
        nav->panScale                = 5.0f;
        nav->lookScale               = inca::math::PI<scalar_t>() / 64.0 / 4;

    CompositeWidgetPtr composite(new CompositeWidget());
    this->_renderView = RenderingViewPtr(new RenderingView(new ChromosomeRendering()));
    this->widget = nav;
        nav->widget = composite;
            composite->view = this->_renderView;

    // Store the map we'll be trying to approximate
    setMapRasterization(mr);

    // Set up the selected object parameters
    _selectedLOD = TerrainLOD::minimum();
    _selectedChromosomeIndex = 0;
}


// MapRasterization accessor functions
MapRasterizationPtr GeneticsWindow::mapRasterization() {
    return _heightfieldGA.mapRasterization();
}
void GeneticsWindow::setMapRasterization(MapRasterizationPtr mr) {
    _heightfieldGA.setMapRasterization(mr);
}


// Control over which chromosome we're looking at
SizeType GeneticsWindow::chromosomeCount() const {
    return _heightfieldGA.populationSize();
}
IndexType GeneticsWindow::selectedChromosomeIndex() const {
    return _selectedChromosomeIndex;
}
void GeneticsWindow::setSelectedChromosomeIndex(IndexType idx) {
    _selectedChromosomeIndex = idx;
    _renderView->object()->setChromosome(_heightfieldGA.chromosome(idx));
    requestRedisplay();
}
void GeneticsWindow::selectPreviousChromosome() {
    setSelectedChromosomeIndex((selectedChromosomeIndex() + chromosomeCount() - 1)
                               % chromosomeCount());
}
void GeneticsWindow::selectNextChromosome() {
    setSelectedChromosomeIndex((selectedChromosomeIndex() + 1) % chromosomeCount());
}


// Control over visible LOD, also triggering GA
TerrainLOD GeneticsWindow::selectedLOD() const {
    return _selectedLOD;
}
void GeneticsWindow::setSelectedLOD(TerrainLOD targetLOD) {
    // Make sure it's a legal value
    if (targetLOD == TerrainLOD_Underflow || targetLOD == TerrainLOD_Overflow)
        return;

    TerrainLOD startLOD = _heightfieldGA.currentLOD();
    if (targetLOD < startLOD)
        INCA_WARNING("Going 'back' not supported yet")
    else if (targetLOD > startLOD && ! _heightfieldGA.running()) {
        INCA_DEBUG("Going from " << (startLOD + 1) << " to " << targetLOD)
        _heightfieldGA.run(startLOD + 1, targetLOD);
        _selectedLOD = targetLOD;
        setSelectedChromosomeIndex(selectedChromosomeIndex());
    }
}
void GeneticsWindow::selectPreviousLOD() {
    setSelectedLOD(selectedLOD() - 1);
}
void GeneticsWindow::selectNextLOD() {
    setSelectedLOD(selectedLOD() + 1);
}

// HACK!
void GeneticsWindow::display() {
    static bool init = false;
    if (this->_renderer && ! init) {
        init = true;
        // Black background, auto-cleared
        renderer().rasterizer().setBackgroundColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
        renderer().addAutoClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer().rasterizer().setDepthBufferingEnabled(true);

        // Enable lighting and shading
        Renderer::LightingUnit & light0 = renderer().lightingUnit(0);
        light0.setPosition(Point3D(1000.0f, 1000.0f, 2000.0f));
        light0.setDiffuseColor(Color(0.6f, 0.6f, 0.6f, 1.0f));
        light0.setEnabled(true);

        Renderer::LightingUnit & light1 = renderer().lightingUnit(1);
        light1.setPosition(Point3D(-1000.0f, 0.0f, 1000.0f));
        light1.setDiffuseColor(Color(0.2f, 0.2f, 0.2f, 1.0f));
        light1.setEnabled(true);

    }
    WINDOW(GUI_TOOLKIT)::display();
}

void GeneticsWindow::key(unsigned char k, int x, int y) {
    switch(k) {
    case 27:
        inca::ui::Application::instance().exit(0, "User pressed ESC");
        break;
    default:
        WINDOW(GUI_TOOLKIT)::key(k, x, y);
        break;
    }
}

void GeneticsWindow::special(int k, int x, int y) {
    switch (k) {
    case GLUT_KEY_F1:
        this->_renderView->object()->toggle("Points");
        requestRedisplay();
        break;
    case GLUT_KEY_F2:
        this->_renderView->object()->toggle("Wireframe");
        requestRedisplay();
        break;
    case GLUT_KEY_F3:
        this->_renderView->object()->toggle("Polygons");
        requestRedisplay();
        break;
    case GLUT_KEY_F4:
        this->_renderView->object()->toggle("Lighting");
        requestRedisplay();
        break;
    case GLUT_KEY_UP:
        selectNextLOD();
        break;
    case GLUT_KEY_DOWN:
        selectPreviousLOD();
        break;
    case GLUT_KEY_LEFT:
        selectPreviousChromosome();
        break;
    case GLUT_KEY_RIGHT:
        selectNextChromosome();
        break;
    default:
        WINDOW(GUI_TOOLKIT)::special(k, x, y);
        break;
    }
}
