// Import class definition
#include "HeightfieldWindow.hpp"

#define FULL_SCREEN false
#define WINDOW_WIDTH    400
#define WINDOW_HEIGHT   300


// Declare the specialized Widget subclass that will serve as the top-level
// widget for this kind of Window
namespace terrainosaurus {
    // Forward declaration
    class HeightfieldWindowWidget;
    typedef shared_ptr<HeightfieldWindowWidget> HeightfieldWindowWidgetPtr;
}

#define GL_HPP_IMPORT_GLUT 1
#include <inca/integration/opengl/GL.hpp>

#include <terrainosaurus/genetics/HeightfieldGA.hpp>

// Import UI & rendering object definitions
#include <inca/ui/PassThruWidget.hpp>
#include <inca/ui/CameraNavigationWidget.hpp>
#include "ObjectRenderingView.hpp"
#include <terrainosaurus/rendering/HeightfieldRendering.hpp>
#include <terrainosaurus/rendering/ChromosomeRendering.hpp>

using namespace terrainosaurus;
using namespace inca;
using namespace inca::ui;
using namespace inca::world;

// Declare specialized View types
typedef ObjectRenderingView<HeightfieldRendering>   HeightfieldView;
typedef ObjectRenderingView<ChromosomeRendering>    ChromosomeView;
typedef shared_ptr<HeightfieldView>                 HeightfieldViewPtr;
typedef shared_ptr<ChromosomeView>                  ChromosomeViewPtr;


class terrainosaurus::HeightfieldWindowWidget : public PassThruWidget {
public:
    // Constructor
    explicit HeightfieldWindowWidget(const std::string & nm = "")
            : PassThruWidget(),
              _selectedLOD(TerrainLOD::minimum()),
              _selectedChromosomeIndex(0) { name = nm; }

    void construct() {
        // Add on the camera navigation control widget
        CameraPtr camera(new PerspectiveCamera());
            camera->transform()->position = Point3D(1000.0f, 1000.0f, 500.0f);
            camera->nearClip = 50.0f;
            camera->farClip  = 100000.0f;
            camera->transform()->lookAt(Point3D(0.0f, 0.0f, 0.0f),
                                        Vector3D(0.0f, 0.0f, 1.0f));
        _cameraNav.reset(new CameraNavigationWidget(camera, "Camera Nav"));
            _cameraNav->longitudinalMotionScale = 30.0f;
            _cameraNav->lateralMotionScale      = 30.0f;
            _cameraNav->verticalMotionScale     = 30.0f;
            _cameraNav->panScale                = 5.0f;
            _cameraNav->lookScale               = inca::math::PI<scalar_t>() / 64.0 / 4;
        this->widget = _cameraNav;

        // Create the Heightfield & Chromosome views, and stick them on
        _multiplexor.reset(new MultiplexorWidget("View Multiplexor"));

        CompositeWidgetPtr compositeHF(new CompositeWidget("Heightfield View"));
        this->_heightfieldView.reset(new HeightfieldView(new HeightfieldRendering(),
                                                         "Heightfield Rendering"));
        _multiplexor->addWidget(compositeHF);
        compositeHF->view = _heightfieldView;

        CompositeWidgetPtr compositeC(new CompositeWidget("Chromosome View"));
        this->_chromosomeView.reset(new ChromosomeView(new ChromosomeRendering(),
                                                       "Chromosome Rendering"));
        _multiplexor->addWidget(compositeC);
        compositeC->view = _chromosomeView;

        _cameraNav->widget = _multiplexor;
    }


    void setMapRasterization(MapRasterizationPtr mr) {
        _heightfieldGA.setMapRasterization(mr);
        _multiplexor->selectWidget("Chromosome");
    }
    void setTerrainSample(TerrainSamplePtr ts) {
        _multiplexor->selectWidget("Heightfield");
    }

    SizeType chromosomeCount() const {
        return _heightfieldGA.populationSize();
    }
    IndexType selectedChromosomeIndex() const {
        return _selectedChromosomeIndex;
    }
    void setSelectedChromosomeIndex(IndexType idx) {
        _selectedChromosomeIndex = idx;
        _chromosomeView->object()->setChromosome(_heightfieldGA.chromosome(idx));
        requestRedisplay();
    }
    void selectPreviousChromosome() {
        setSelectedChromosomeIndex((selectedChromosomeIndex() + chromosomeCount() - 1)
                                % chromosomeCount());
    }
    void selectNextChromosome() {
        setSelectedChromosomeIndex((selectedChromosomeIndex() + 1) % chromosomeCount());
    }


    // Control over visible LOD, also triggering GA
    TerrainLOD selectedLOD() const { return _selectedLOD; }
    void setSelectedLOD(TerrainLOD targetLOD) {
        // Make sure it's a legal value
        if (targetLOD == TerrainLOD_Underflow || targetLOD == TerrainLOD_Overflow)
            return;

        TerrainLOD startLOD = _heightfieldGA.currentLOD();

        if (targetLOD < startLOD)
            INCA_WARNING("Going 'back' not supported yet")
        else if (_heightfieldGA.running())
            INCA_INFO("The GA is already running. Please wait")
        else {
            _selectedLOD = targetLOD;
            if (targetLOD >= startLOD) {
                INCA_DEBUG("Going from " << startLOD << " to " << targetLOD)
//                _heightfieldGA.run(startLOD, targetLOD);
//                setSelectedChromosomeIndex(selectedChromosomeIndex());
//                requestRedisplay();
            }
        }
    }
    void selectPreviousLOD() { setSelectedLOD(selectedLOD() - 1); }
    void selectNextLOD()     { setSelectedLOD(selectedLOD() + 1); }

    void initializeView() {
        // Black background, auto-cleared
        renderer().addAutoClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer().rasterizer().setBackgroundColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
        renderer().rasterizer().setDepthBufferingEnabled(true);

        // Enable lighting and shading
        Renderer::LightingUnit & light0 = renderer().lightingUnit(0);
//        light0.setPosition(Point3D(1000.0f, 1000.0f, 2000.0f));
        light0.setPosition(Point3D(0.0f, 0.0f, 1000.0f));
        light0.setDiffuseColor(Color(0.6f, 0.6f, 0.6f, 1.0f));
        light0.setEnabled(true);

//        Renderer::LightingUnit & light1 = renderer().lightingUnit(1);
//        light1.setPosition(Point3D(-1000.0f, 0.0f, 1000.0f));
//        light1.setDiffuseColor(Color(0.2f, 0.2f, 0.2f, 1.0f));
//        light1.setEnabled(true);
    }

    void keyPressed(KeyCode k, Pixel px) {
        switch(k) {
        case KeyEscape:
            inca::ui::Application::instance().exit(0, "User pressed ESC");
            break;
        case KeyF1:
            this->_heightfieldView->object()->toggle("Points");
            this->_chromosomeView->object()->toggle("Points");
            requestRedisplay();
            break;
        case KeyF2:
            this->_heightfieldView->object()->toggle("Wireframe");
            this->_chromosomeView->object()->toggle("Wireframe");
            requestRedisplay();
            break;
        case KeyF3:
            this->_heightfieldView->object()->toggle("Polygons");
            this->_chromosomeView->object()->toggle("Polygons");
            requestRedisplay();
            break;
        case KeyF4:
            this->_heightfieldView->object()->toggle("Lighting");
            this->_chromosomeView->object()->toggle("Lighting");
            requestRedisplay();
            break;
        case KeyUp:
            selectNextLOD();
            break;
        case KeyDown:
            selectPreviousLOD();
            break;
        case KeyLeft:
            selectPreviousChromosome();
            break;
        case KeyRight:
            selectNextChromosome();
            break;
        case KeySpace:
            _multiplexor->selectNextWidget();
            INCA_INFO("Showing Widget: " << _multiplexor->selectedWidgetName())
            break;
        case KeyR:
            INCA_DEBUG("Trying to redisplay")
            getContainingWindow()->requestRedisplay();
//            requestRedisplay();
            break;
        default:
            PassThruWidget::keyPressed(k, px);
            break;
        }
    }

protected:
    // Data objects
    HeightfieldGA       _heightfieldGA;
    IndexType           _selectedChromosomeIndex;
    TerrainLOD          _selectedLOD;

    // Sub-Widgets we need to hold on to
    CameraNavigationWidgetPtr   _cameraNav;
    MultiplexorWidgetPtr        _multiplexor;
    ChromosomeViewPtr           _chromosomeView;
    HeightfieldViewPtr          _heightfieldView;
};



HeightfieldWindow::HeightfieldWindow(TerrainSamplePtr ts,
                                     const std::string & title)
            : WINDOW(GUI_TOOLKIT)(title), _terrainSample(ts) { }

HeightfieldWindow::HeightfieldWindow(MapRasterizationPtr mr,
                                     const std::string & title)
            : WINDOW(GUI_TOOLKIT)(title), _mapRasterization(mr) { }


// GUI setup function
void HeightfieldWindow::construct() {
    INCA_DEBUG("Constructed? " << this->constructed)
    INCA_DEBUG("Calling HFW construct")

    // Set up the window
    setSize(Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
    setFullScreen(FULL_SCREEN);

    GL::glutSetCursor(GLUT_CURSOR_NONE);

    // Set up the renderer
    this->_renderer.reset(new Renderer());

    // Construct the top-level widget
    HeightfieldWindowWidgetPtr w;
    w.reset(new HeightfieldWindowWidget("Heightfield Window"));
    this->widget = w;

    // XXX This two-phase construction ought to be a part of the
    // inca UI framework.
    w->construct();

    // Pass along anything that we got from our constructor
    if (_mapRasterization)
        w->setMapRasterization(_mapRasterization);
    if (_terrainSample)
        w->setTerrainSample(_terrainSample);
}


// MapRasterization accessor functions
MapRasterizationPtr HeightfieldWindow::mapRasterization() {
    return _mapRasterization;
}
void HeightfieldWindow::setMapRasterization(MapRasterizationPtr mr) {
    _mapRasterization = mr;
    if (widget) {
        HeightfieldWindowWidgetPtr hfw = static_pointer_cast<HeightfieldWindowWidget>(widget());
        hfw->setMapRasterization(mr);
    }
}

// TerrainSample accessor functions
TerrainSamplePtr HeightfieldWindow::terrainSample() {
    return _terrainSample;
}
void HeightfieldWindow::setTerrainSample(TerrainSamplePtr ts) {
    _terrainSample = ts;
    if (widget) {
        HeightfieldWindowWidgetPtr hfw = static_pointer_cast<HeightfieldWindowWidget>(widget());
        hfw->setTerrainSample(ts);
    }
}
