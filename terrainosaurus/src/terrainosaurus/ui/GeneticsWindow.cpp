#define BOOST_NO_MEMBER_TEMPLATE_FRIENDS 1

// Import class definition
#include "GeneticsWindow.hpp"

#define GL_HPP_IMPORT_GLUT 1
#include <inca/integration/opengl/GL.hpp>

#include <inca/ui/CameraNavigationWidget.hpp>

using namespace terrainosaurus;
using namespace inca::ui;
using namespace inca::world;

// XXX Should this go somewhere else?
struct null_deleter {
    void operator()(void const *) { }
};


// Constructor taking an image
GeneticsWindow::GeneticsWindow(TerrainChromosome & c,
                               const std::string & title)
            : WINDOW(GUI_TOOLKIT)(title) {
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
    this->renderView = RenderingViewPtr(new RenderingView(new ChromosomeRendering()));
    this->widget = nav;
        nav->widget = composite;
            composite->view = this->renderView;

    // Finally, load the chromosome
    setChromosome(c);
}

// TerrainChromosome accessor functions
TerrainChromosome & GeneticsWindow::chromosome() {
    return *_chromosome;
}
void GeneticsWindow::setChromosome(TerrainChromosome & c) {
    // Stop listening to the old chromosome
    if (_chromosome)
        _chromosome->removeGAListener(this->renderView->object());

    _chromosome = &c;
    this->renderView->object()->setChromosome(chromosome());

    // Start listening to the new one
    if (_chromosome)
        _chromosome->addGAListener(this->renderView->object());
    requestRedisplay();
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

        setFullScreen(true);
        GL::glutSetCursor(GLUT_CURSOR_NONE);
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
        this->renderView->object()->toggle("Points");
        requestRedisplay();
        break;
    case GLUT_KEY_F2:
        this->renderView->object()->toggle("Wireframe");
        requestRedisplay();
        break;
    case GLUT_KEY_F3:
        this->renderView->object()->toggle("Polygons");
        requestRedisplay();
        break;
    case GLUT_KEY_F4:
        this->renderView->object()->toggle("Lighting");
        requestRedisplay();
        break;
    default:
        WINDOW(GUI_TOOLKIT)::special(k, x, y);
        break;
    }
}
