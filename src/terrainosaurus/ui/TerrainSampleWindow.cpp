/** -*- C++ -*-
 *
 * \file    TerrainSampleWindow.cpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "TerrainSampleWindow.hpp"

// Initial configuration constants
#define WINDOW_SIZE         Dimension(400, 300)
#define FULL_SCREEN         false

#define BACKGROUND_COLOR    Color(0.0f, 0.0f, 0.0f, 1.0f)
#define PATTERN_COLOR       Color(0.8f, 0.4f, 0.4f, 1.0f)
#define SAMPLE_COLOR        Color(0.5f, 0.8f, 0.8f, 1.0f)

// Key light
#define LIGHT0_ON           true
#define LIGHT0_POSITION     Point3D(0.0f, 10000.0f, 500.0f)
#define LIGHT0_DIFFUSE      (Color(1.0f, 0.90f, 0.58f, 1.0f) * 1.0f)
#define LIGHT0_AMBIENT      (Color(1.0f, 0.55f, 0.29f, 1.0f) * 0.2f)
#define LIGHT0_SPECULAR     (Color(1.0f, 1.0f, 1.0f, 1.0f)   * 1.0f)
//#define LIGHT0_DIFFUSE      Color(0.6f, 0.6f, 0.6f, 1.0f)
//#define LIGHT0_AMBIENT      Color(0.1f, 0.1f, 0.1f, 1.0f)
//#define LIGHT0_SPECULAR     Color(1.0f, 1.0f, 1.0f, 1.0f)

// Fill light
#define LIGHT1_ON           false
#define LIGHT1_POSITION     Point3D(0.0f, 0.0f, 1000.0f)
#define LIGHT1_DIFFUSE      (Color(0.97f, 0.43f, 0.39f, 1.0f) * 0.1f)
#define LIGHT1_AMBIENT      (Color(1.0f, 1.0f, 1.0f, 1.0f)    * 0.0f)
#define LIGHT1_SPECULAR     (Color(1.0f, 1.0f, 1.0f, 1.0f)    * 0.0f)
//#define LIGHT1_DIFFUSE      Color(0.2f, 0.2f, 0.2f, 1.0f)
//#define LIGHT1_AMBIENT      Color(0.0f, 0.0f, 0.0f, 1.0f)
//#define LIGHT1_SPECULAR     Color(1.0f, 1.0f, 1.0f, 1.0f)

// Light debugging
#define LIGHT_VECTORS_ON    false
#define LIGHT_VECTOR_LENGTH 200.0f

// Default camera parameters
#define CAMERA_POSITION     Point3D(10000.0f, 5000.0f, 5000.0f)
#define CAMERA_LOOK_AT      Point3D(0.0f, 0.0f, 0.0f)
#define CAMERA_UP           Vector3D(0.0f, 0.0f, 1.0f)
#define CAMERA_NEAR_CLIP    50.0f
#define CAMERA_FAR_CLIP     100000.0f
//#define CAMERA_FOV          15.0f

#define BORDER_WIDTH        2

// Declare the specialized Widget subclass that will serve as the top-level
// widget for this kind of Window
namespace terrainosaurus {
    // Forward declaration
    class TerrainSampleWindowWidget;
    typedef shared_ptr<TerrainSampleWindowWidget> TerrainSampleWindowWidgetPtr;
}


// Import TerrainSample generation algorithm and utilities
#include <terrainosaurus/genetics/terrain-operations.hpp>
#include <terrainosaurus/genetics/HeightfieldGA.hpp>

// Import UI & rendering object definitions
#include <terrainosaurus/TerrainosaurusApplication.hpp>
#include <inca/ui/widgets/WindowControlWidget.hpp>
#include <inca/ui/widgets/PassThruWidget.hpp>
#include <inca/ui/widgets/CameraNavigationWidget.hpp>
#include <inca/ui/widgets/MultiplexorWidget.hpp>
#include <inca/ui/widgets/LightingWidget.hpp>
#include <inca/ui/widgets/LayeringWidget.hpp>
#include "ObjectRenderingView.hpp"
#include <terrainosaurus/rendering/TerrainSampleRendering.hpp>
#include <terrainosaurus/rendering/ChromosomeRendering.hpp>
#include <terrainosaurus/rendering/SkyBox.hpp>

using namespace terrainosaurus;
using namespace inca;
using namespace inca::ui;
using namespace inca::world;

// HACK! Remove dependence on GL_??_BUFFER_BIT
#include <inca/integration/opengl/GL.hpp>


// Declare specialized View types
typedef ObjectRenderingView<TerrainSampleRendering> TerrainSampleView;
typedef ObjectRenderingView<ChromosomeRendering>    ChromosomeView;
typedef ObjectRenderingView<SkyBox>                 SkyBoxView;
typedef shared_ptr<TerrainSampleView>               TerrainSampleViewPtr;
typedef shared_ptr<ChromosomeView>                  ChromosomeViewPtr;
typedef shared_ptr<SkyBoxView>                      SkyBoxViewPtr;


class terrainosaurus::TerrainSampleWindowWidget : public WindowControlWidget {
/*---------------------------------------------------------------------------*
 | Constructors & initialization
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    explicit TerrainSampleWindowWidget(const std::string & nm = std::string())
            : WindowControlWidget(nm),
              _selectedLOD(TerrainLOD_Underflow),
              _selectedChromosomeIndex(0) { }

    void construct();

protected:
    // Sub-Widgets we need to hold on to
    LayeringWidgetPtr           _layers;
    MultiplexorWidgetPtr        _multiplexor;
    ChromosomeViewPtr           _chromosomeView;
    TerrainSampleViewPtr        _terrainView, _patternView;


/*---------------------------------------------------------------------------*
 | Data object accessors
 *---------------------------------------------------------------------------*/
public:
    TerrainSamplePtr      terrainSample()       { return _terrainSample; }
    TerrainSampleConstPtr terrainSample() const { return _terrainSample; }
    void setViewedTerrain(TerrainSamplePtr ts) {
        INCA_INFO("TerrainSampleWindow: Entering view mode")
        _terrainSample = ts;
        _patternSample.reset();
        _multiplexor->selectWidget("Terrain View");
        
        // Choose the best LOD to start with
        ts->ensureLoaded(TerrainLOD::minimum());
        TerrainLOD startLOD = ts->nearestLoadedLODBelow(TerrainLOD_Overflow);
        if (startLOD == TerrainLOD_Underflow)
            startLOD = TerrainLOD::minimum();
        setSelectedLOD(startLOD);
    }

    TerrainSamplePtr      patternSample()       { return _patternSample; }
    TerrainSampleConstPtr patternSample() const { return _patternSample; }
    void setGeneratedTerrain(TerrainSamplePtr ts, TerrainSamplePtr ps) {
        INCA_INFO("TerrainSampleWindow: Entering generate mode")
        _patternSample = ps;
        _terrainSample = ts;
        _heightfieldGA.setPatternSample(ps);
        _heightfieldGA.setTerrainSample(ts);
        _multiplexor->selectWidget("Terrain View");
        
        // Choose the best LOD to start with
        TerrainLOD startLOD = ps->nearestLoadedLODBelow(TerrainLOD_Overflow);
        if (startLOD == TerrainLOD_Underflow) {
            INCA_DEBUG("Generating base LOD via naive blend of "
                       << BORDER_WIDTH << " pixels")
            naiveBlend((*ps)[TerrainLOD::minimum()], BORDER_WIDTH);
            INCA_DEBUG("Done generating base LOD")
            (*ts)[TerrainLOD::minimum()] = (*ps)[TerrainLOD::minimum()];
            startLOD = TerrainLOD::minimum();
        }
        setSelectedLOD(startLOD);
    }

    // We're in view-only mode if there is no pattern (=> no GA)
    bool viewOnlyMode() const { return ! _patternSample; }
    
protected:
    HeightfieldGA       _heightfieldGA;
    TerrainSamplePtr    _terrainSample;
    TerrainSamplePtr    _patternSample;


/*---------------------------------------------------------------------------*
 | Chromosome selection functions
 *---------------------------------------------------------------------------*/
public:
    SizeType chromosomeCount() const {
        return _heightfieldGA.populationSize();
    }
    IndexType selectedChromosomeIndex() const {
        return _selectedChromosomeIndex;
    }
    void setSelectedChromosomeIndex(IndexType idx) {
        if (! viewOnlyMode()) {
            _selectedChromosomeIndex = idx;
            _loadChromosome(_heightfieldGA.chromosome(idx));
            requestRedisplay();
        }
    }
    void selectPreviousChromosome() {
        setSelectedChromosomeIndex((selectedChromosomeIndex() + chromosomeCount() - 1)
                                % chromosomeCount());
    }
    void selectNextChromosome() {
        setSelectedChromosomeIndex((selectedChromosomeIndex() + 1) % chromosomeCount());
    }

protected:
    IndexType _selectedChromosomeIndex;


/*---------------------------------------------------------------------------*
 | LOD selection functions
 *---------------------------------------------------------------------------*/
public:
    TerrainLOD selectedLOD() const { return _selectedLOD; }
    void selectPreviousLOD() { setSelectedLOD(selectedLOD() - 1); }
    void selectNextLOD()     { setSelectedLOD(selectedLOD() + 1); }
    void setSelectedLOD(TerrainLOD targetLOD) {
        // Make sure it's a legal value
        if (targetLOD == TerrainLOD_Underflow || targetLOD == TerrainLOD_Overflow)
            return;

        // Ensure that each LOD we care about is valid (this may force the
        // TS to resample if in view-only mode, or it may force the GA to be
        // run for one or more LODs if we're in terrain generation mode). This
        // returns 'true' if successful, and false if it failed (e.g., if the
        // GA is currently running, or if one or more LODs can't be created)
        if (_validateLODs(_selectedLOD, targetLOD)) {
            INCA_DEBUG("Selecting " << targetLOD)

            // Set the new LOD
            _selectedLOD = targetLOD;
            
            // Load the new LOD for the HF viewer
            _loadTerrainLOD((*terrainSample())[selectedLOD()]);
            
            // Load the new pattern LOD, if it exists
            if (patternSample())
                _loadPatternLOD((*patternSample())[selectedLOD()]);
            
            // If we have a GA, load the best chromosome into the C viewer
            if (! viewOnlyMode() && _selectedLOD == _heightfieldGA.currentLOD()) {
                IndexType idx = _heightfieldGA.strongestChromosomeIndex();
                _loadChromosome(_heightfieldGA.chromosome(idx));
            }
            
            // Make sure it shows up
            requestRedisplay();
        }
    }

protected:
    // Make sure that valid data exists for the specified range of LODs.
    // Depending on the current mode (view-only or terrain-generating), this
    // may force a load or resample, or it may invoke the GA.
    // Returns true if all is well, or false if the requested LODs cannot be
    // validated (i.e., the GA is busy right now).
    bool _validateLODs(TerrainLOD startLOD, TerrainLOD endLOD);
    
    TerrainLOD _selectedLOD;    // The currently-selected LOD


/*---------------------------------------------------------------------------*
 | Rendering & event-handling functions
 *---------------------------------------------------------------------------*/
public:
    void keyPressed(const KeyEvent & e);

protected:
    // Load a chromosome into the chromosome viewer
    void _loadChromosome(HeightfieldGA::Chromosome & c) {
        INCA_DEBUG("Selecting chromosome " << _heightfieldGA.indexOf(c) << '\n'
                   << "   Overall fitness: " << c.fitness().overall() << '\n'
                   << "   Similarity:      " << c.fitness().similarity() << '\n'
                   << "   Compatibility:   " << c.fitness().compatibility())
        _chromosomeView->object()->setChromosome(c);    
    }
    
    // Load a sample LOD or a GA-generated LOD into the HF viewer
    void _loadTerrainLOD(const TerrainSample::LOD & ts) {
        _terrainView->object()->load(ts);
    }

    // Load a pattern LOD into the pattern viewer
    void _loadPatternLOD(const TerrainSample::LOD & ts) {
        _patternView->object()->load(ts);
    }
};


// Assemble the GUI and scene objects for this window
void TerrainSampleWindowWidget::construct() {
    INCA_DEBUG("Constructing TS Win")
    // Configure the camera
    PerspectiveCameraPtr camera(new PerspectiveCamera());
        camera->transform()->position   = CAMERA_POSITION;
        camera->nearClip                = CAMERA_NEAR_CLIP;
        camera->farClip                 = CAMERA_FAR_CLIP;
        camera->transform()->lookAt(CAMERA_LOOK_AT, CAMERA_UP);

    // Create the camera navigation widget
    CameraNavigationWidgetPtr nav(new CameraNavigationWidget(camera, "Camera Nav"));
        nav->longitudinalMotionScale = 30.0f;
        nav->lateralMotionScale      = 30.0f;
        nav->verticalMotionScale     = 30.0f;
        nav->panScale                = 5.0f;
        nav->lookScale               = inca::math::PI<scalar_t>() / 64.0 / 4;

    // Create the scene lighting
    LightingWidgetPtr lite(new LightingWidget("Scene Lighting"));
        lite->setLightEnabled(0, LIGHT0_ON);
        lite->setPosition(0, LIGHT0_POSITION);
        lite->setAmbientColor(0, LIGHT0_AMBIENT);
        lite->setDiffuseColor(0, LIGHT0_DIFFUSE);
        lite->setSpecularColor(0, LIGHT0_SPECULAR);
        lite->setLightEnabled(1, LIGHT1_ON);
        lite->setPosition(1, LIGHT1_POSITION);
        lite->setAmbientColor(1, LIGHT1_AMBIENT);
        lite->setDiffuseColor(1, LIGHT1_DIFFUSE);
        lite->setSpecularColor(1, LIGHT1_SPECULAR);
        lite->setDrawingLightVectors(LIGHT_VECTORS_ON);
        lite->setLightVectorLength(LIGHT_VECTOR_LENGTH);

    // Create the sky box
    SkyBoxViewPtr sky(new SkyBoxView(new SkyBox(), "Sky Box"));
    
    // Create the object rendering widgets & the mux to switch between them
    _multiplexor.reset(new MultiplexorWidget("View Multiplexor"));
    _terrainView.reset(new TerrainSampleView(new TerrainSampleRendering(),
                                             "Terrain View"));
    _patternView.reset(new TerrainSampleView(new TerrainSampleRendering(),
                                             "Pattern View"));
    _chromosomeView.reset(new ChromosomeView(new ChromosomeRendering(),
                                             "Chromosome View"));

    // Create the layering widget that will composite the sky & objects
    _layers.reset(new LayeringWidget("Layers"));

    // Black background, auto-cleared
    renderer().addAutoClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer().rasterizer().setBackgroundColor(BACKGROUND_COLOR);
    renderer().rasterizer().setDepthBufferingEnabled(true);

    // Construct the widget chain
    this->widget = nav;
        nav->widget = lite;
            lite->widget = _layers;
                _layers->addWidget(sky);
                _layers->addWidget(_multiplexor);
//    this->widget = lighting;
//        lighting->widget = nav;
//            nav->widget = _multiplexor;
                    _multiplexor->addWidget(_terrainView);
                    _multiplexor->addWidget(_patternView);
                    _multiplexor->addWidget(_chromosomeView);
                
    // Make sure we're all set up
    //setSelectedLOD(TerrainLOD::minimum());

    requestRedisplay();
}


// Make sure that valid data exists for the specified range of LODs. Depending
// on the current mode (view-only or terrain-generating), this may force a
// load or resample, or it may invoke the GA.
bool TerrainSampleWindowWidget::_validateLODs(TerrainLOD startLOD,
                                              TerrainLOD endLOD) {
    // Only validate legal LODs
    if (endLOD == TerrainLOD_Underflow)         return true;
    else if (endLOD == TerrainLOD_Overflow)     --endLOD;
    if (startLOD == TerrainLOD_Overflow)        return true;
    else if (startLOD == TerrainLOD_Underflow)  ++startLOD;

    if (viewOnlyMode()) {
        // We're in view-only mode. This means that we're just looking at a
        // multi-rez terrain, with no GA involved.
        TerrainSamplePtr ts = terrainSample();

        // Make sure we have such a thing        
        if (! ts)
            return false;

        // Load each LOD in the range
        terrainSample()->ensureLoaded(startLOD, endLOD);
        return true;
    
    } else {
        // We're in terrain generation mode. This may limit what we can do.
        
        // First, let's see if fulfilling this request would require new
        // LODs to be generated.
        TerrainLOD finestValidLOD = _heightfieldGA.currentLOD() - 1;
        
        // If what we've got already isn't good enough...
        if (finestValidLOD < endLOD) {
            // See if the GA is occupied
            if (_heightfieldGA.running()) {
                INCA_INFO("Unable to generate requested LOD -- GA is busy")
                return false;
            }
            
            // If we got here, then the GA is free and we need it to do
            // some work for us.
            _heightfieldGA.run(endLOD);
        }
        return true;
    }
}            


// Handle application-level key events
void TerrainSampleWindowWidget::keyPressed(const KeyEvent & e) {
    switch(e.keyCode()) {
    case KeyF1:
        this->_patternView->object()->toggle("Points");
        this->_terrainView->object()->toggle("Points");
        this->_chromosomeView->object()->toggle("Points");
        requestRedisplay();
        break;
    case KeyF2:
        this->_patternView->object()->toggle("Wireframe");
        this->_terrainView->object()->toggle("Wireframe");
        this->_chromosomeView->object()->toggle("Wireframe");
        requestRedisplay();
        break;
    case KeyF3:
        this->_patternView->object()->toggle("Polygons");
        this->_terrainView->object()->toggle("Polygons");
        this->_chromosomeView->object()->toggle("Polygons");
        requestRedisplay();
        break;
    case KeyF4:
        this->_patternView->object()->toggle("Normals");
        this->_terrainView->object()->toggle("Normals");
        this->_chromosomeView->object()->toggle("Normals");
        requestRedisplay();
        break;
    case KeyF5:
        this->_patternView->object()->toggle("Fog");
        this->_terrainView->object()->toggle("Fog");
        this->_chromosomeView->object()->toggle("Fog");
        requestRedisplay();
        break;
    case KeyF6:
        this->_layers->toggleLayerVisible(0 /* sky */);;
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
    case KeyT:
        _heightfieldGA.test(LOD_270m);
        break;
    case KeyF: {
        // Find the terrain we're looking at
        TerrainSamplePtr ts;
        if (_multiplexor->selectedWidget()->name() == "Terrain View")
            ts = _terrainSample;
        else if (_multiplexor->selectedWidget()->name() == "Pattern View")
            ts = _patternSample;
        else
            ts = _heightfieldGA.chromosome(selectedChromosomeIndex()).scratchSample();

        // Find the edges in this height field
        TerrainosaurusApplication::instance().createImageWindow((*ts)[selectedLOD()].featureMaps());
        break;
    }
    case KeySpace:
        // Toggle between pattern and chromosome view modes
        if (! viewOnlyMode())
            _multiplexor->selectNextWidget();
        break;
    default:
        WindowControlWidget::keyPressed(e);
        break;
    }
}


TerrainSampleWindow::TerrainSampleWindow(TerrainSamplePtr ts,
                                     const std::string & nm)
            : Window(nm), _terrainSample(ts) { }

TerrainSampleWindow::TerrainSampleWindow(TerrainSamplePtr ts, TerrainSamplePtr ps,
                                     const std::string & nm)
            : Window(nm), _terrainSample(ts), _patternSample(ps) { }


// GUI setup function
void TerrainSampleWindow::construct() {
    // HACK : call superclass version...need other mechanism
    Window::construct();

    // Set up the window
    setSize(WINDOW_SIZE);
    setFullScreen(FULL_SCREEN);

    // Attach the widget chain
    TerrainSampleWindowWidgetPtr w(new TerrainSampleWindowWidget("TerrainSample Window"));
    surface()->setWidget(w);

    // Pass along anything that we got from our constructor
    if (_patternSample)
        w->setGeneratedTerrain(_terrainSample, _patternSample);
    else if (_terrainSample)
        w->setViewedTerrain(_terrainSample);
}


// TerrainSample accessor functions
TerrainSamplePtr TerrainSampleWindow::terrainSample() {
    return _terrainSample;
}
void TerrainSampleWindow::setTerrainSample(TerrainSamplePtr ts) {
    _terrainSample = ts;
    WidgetPtr w = surface()->widget();
    if (w) {
        TerrainSampleWindowWidgetPtr hfw = static_pointer_cast<TerrainSampleWindowWidget>(w);
        if (_patternSample)
            hfw->setGeneratedTerrain(_terrainSample, _patternSample);
        else
            hfw->setViewedTerrain(_terrainSample);
    }
}

// TerrainSample accessor functions
TerrainSamplePtr TerrainSampleWindow::patternSample() {
    return _patternSample;
}
void TerrainSampleWindow::setPatternSample(TerrainSamplePtr ts) {
    _patternSample = ts;
    WidgetPtr w = surface()->widget();
    if (w) {
        TerrainSampleWindowWidgetPtr hfw = static_pointer_cast<TerrainSampleWindowWidget>(w);
        if (_patternSample)
            hfw->setGeneratedTerrain(_terrainSample, _patternSample);
        else
            hfw->setViewedTerrain(_terrainSample);
    }
}
