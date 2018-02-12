/** -*- C++ -*-
 *
 * \file    MapEditorWindow.cpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MapEditorWindow class is a top-level Window subclass for
 *      displaying and editing 2D maps.
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "MapEditorWindow.hpp"


// Initial configuration constants
#define WINDOW_SIZE         Dimension(300, 300)
#define FULL_SCREEN         false

#define BACKGROUND_COLOR    Color(0.2f, 0.2f, 0.2f, 0.0f)

#define CAMERA_POSITION     Point3D(0.0f, 0.0f, 100.0f)
#define CAMERA_LOOK_AT      Point3D(0.0f, 0.0f, 0.0f)
#define CAMERA_UP           Vector3D(0.0f, 1.0f, 0.0f)
#define CAMERA_NEAR_CLIP    50.0f
#define CAMERA_FAR_CLIP     1000.0f
#define CAMERA_WIDTH        15.0f
#define CAMERA_HEIGHT       15.0f

// How many pixels to pad the map when fitMap() is called
#define FIT_PADDING     10

#define H_LOOK_SPEED    0.02f
#define V_LOOK_SPEED    0.005f
#define H_SPEED         50.0f
#define V_SPEED         50.0f
#define MAX_PHI         2.5f


// Declare the specialized Widget subclass that will serve as the top-level
// widget for this kind of Window
namespace terrainosaurus {
    // Forward declaration
    class MapEditorWindowWidget;
    typedef shared_ptr<MapEditorWindowWidget> MapEditorWindowWidgetPtr;
}


// Import application class definition
#include <terrainosaurus/TerrainosaurusApplication.hpp>

// Import rendering object definitions
#include <terrainosaurus/rendering/MapRendering.hpp>
//#include <terrainosaurus/rendering/GridRendering.hpp>


// Import widget definitions
#include <inca/ui/widgets/PassThruWidget.hpp>
#include <inca/ui/widgets/WindowControlWidget.hpp>
#include <inca/ui/widgets/CameraNavigationWidget.hpp>
#include "ObjectRenderingView.hpp"

//#include "ui/MapExplorerWidget.hpp"
//#include "ui/CameraFlyWidget.hpp"
//#include "ui/MapEditWidget.hpp"
//#include "ui/AddFaceWidget.hpp"
//#include "ui/AddVertexWidget.hpp"
//#include "ui/ModifyPropertyWidget.hpp"
//#include "ui/TranslateWidget.hpp"
//#include "ui/RotateWidget.hpp"
//#include "ui/ScaleWidget.hpp"

// Import boundary-refinement GA
#include <terrainosaurus/genetics/BoundaryGA.hpp>


#define GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>

using namespace inca;
using namespace inca::ui;
using namespace inca::world;
using namespace terrainosaurus;

// Declare specialized View types
typedef ObjectRenderingView<MapRendering>           MapView;
//typedef ObjectRenderingView<GridRendering>          GridView;
typedef shared_ptr<MapView>                         MapViewPtr;
//typedef shared_ptr<GridView>                        GridViewPtr;


class terrainosaurus::MapEditorWindowWidget
        : public inca::ui::WindowControlWidget {
/*---------------------------------------------------------------------------*
 | Constructors & initialization
 *---------------------------------------------------------------------------*/
public:
    // Constructor taking a map object to edit
    MapEditorWindowWidget(const std::string & nm = std::string());

    // Second-phase initialization
    void construct();

protected:
    // Sub-Widgets we need to hold on to
    CameraNavigationWidgetPtr   _navigation;
    MapViewPtr                  _mapView;
//    GridViewPtr                 _gridView;


/*---------------------------------------------------------------------------*
 | Data object accessors
 *---------------------------------------------------------------------------*/
public:
    // Map accessors
    MapPtr      map()       { return _map; }
    MapConstPtr map() const { return _map; }
    void setMap(MapPtr m) {
        _map = m;
        _mapView->object()->setMap(m);
    }
    
    // Persistent selection accessors
    MeshSelectionPtr persistentSelection() {
        return _persistentSelection;
    }
    MeshSelectionConstPtr persistentSelection() const {
        return _persistentSelection;
    }
    void setPersistentSelection(MeshSelectionPtr s) {
        _persistentSelection = s;
        _mapView->object()->setPersistentSelection(s);
    }
    
    // Transient selection accessors
    MeshSelectionPtr      transientSelection() {
        return _transientSelection;
    }
    MeshSelectionConstPtr transientSelection() const {
        return _transientSelection;
    }
    void setTransientSelection(MeshSelectionPtr s) {
        _transientSelection = s;
        _mapView->object()->setTransientSelection(s);
    }
    
    // Refine a single map boundary
    void refineBoundary(Map::Edge & e, TerrainLOD targetLOD);
    
    // Refine a set of map boundaries
    void refineBoundaries(const MeshSelection & edges, TerrainLOD targetLOD);
    
    // Refine the entire map
    void refineMap(TerrainLOD targetLOD);
    
    // Rasterize the map and make a height field
    void generateHeightfieldForView();

protected:
    BoundaryGA          _boundaryGA;
    MapPtr              _map;
    MeshSelectionPtr    _persistentSelection,
                        _transientSelection;


/*---------------------------------------------------------------------------*
 | Rendering & event-handling functions
 *---------------------------------------------------------------------------*/
public:
    // Fit the map into the window, zooming and translating as needed
    void fitMap();

    // Event handlers
//    void componentResized(const ComponentEvent & e);
    void keyPressed(const KeyEvent & e);    
};


// Constructor
MapEditorWindowWidget::MapEditorWindowWidget(const std::string & nm)
        : WindowControlWidget(nm) {

    // Set up the parameters for the GA from the TApp's defaults
    const TerrainosaurusApplication & app = TerrainosaurusApplication::instance();
    _boundaryGA.setPopulationSize(app.boundaryGAPopulationSize());
    _boundaryGA.setEvolutionCycles(app.boundaryGAEvolutionCycles());
    _boundaryGA.setSelectionRatio(app.boundaryGASelectionRatio());
    _boundaryGA.setElitismRatio(app.boundaryGAElitismRatio());
    _boundaryGA.setMutationProbability(app.boundaryGAMutationProbability());
    _boundaryGA.setMutationRatio(app.boundaryGAMutationRatio());
    _boundaryGA.setCrossoverProbability(app.boundaryGACrossoverProbability());
    _boundaryGA.setCrossoverRatio(app.boundaryGACrossoverRatio());
    _boundaryGA.setMaxAbsoluteAngle(app.boundaryGAMaxAbsoluteAngle());
}

// Second-phase initialization for the MapEditorWindowWidget
void MapEditorWindowWidget::construct() {
    INCA_DEBUG("Constructing ME Win")
    // Set up the camera
    OrthographicCameraPtr camera(new OrthographicCamera());
    camera->transform()->position   = CAMERA_POSITION;
    camera->viewWidth               = CAMERA_WIDTH;
    camera->viewHeight              = CAMERA_HEIGHT;
    camera->nearClip                = CAMERA_NEAR_CLIP;
    camera->farClip                 = CAMERA_FAR_CLIP;               
    camera->transform()->lookAt(CAMERA_LOOK_AT, CAMERA_UP);


    // Construct the widget chain
    _navigation.reset(new CameraNavigationWidget(camera, "Map Navigation"));
        _navigation->enableLook  = false;
        _navigation->enablePitch = false;
        _navigation->enableYaw   = false;
        _navigation->enableLongitudinalMotion = false;
    _mapView.reset(new MapView(new MapRendering(), "Map"));

    this->widget        = _navigation;
    _navigation->widget = _mapView;

    
    // Configure the renderer
    Renderer & r = this->renderer();
    r.addAutoClearBuffer(GL_COLOR_BUFFER_BIT);
    r.rasterizer().setBackgroundColor(BACKGROUND_COLOR);
    r.rasterizer().setDepthBufferingEnabled(false);
    r.rasterizer().setLightingEnabled(false);
//     r.rasterizer().setLineSmoothingEnabled(true);
//     r.rasterizer().setPointSmoothingEnabled(true);
//     r.rasterizer().setAlphaBlendingEnabled(true);

    if (_map)
        fitMap();
}


void MapEditorWindowWidget::refineMap(TerrainLOD targetLOD) {
    MeshSelection s(_map);
    s.setElementType(MeshSelection::Edges);
    s.selectAll();
    refineBoundaries(s, targetLOD);
}

void MapEditorWindowWidget::refineBoundaries(const MeshSelection & set,
                                             TerrainLOD targetLOD) {
    if (set.elementType() == MeshSelection::Edges) {
        MeshSelection::const_iterator it;
        for (it = set.begin(); it != set.end(); ++it)
            refineBoundary(*_map->edge(*it), targetLOD);
    } else {
        INCA_WARNING(__FUNCTION__ << "(MS): Current selection mode must "
                     "be 'Edges'")
    }
}

void MapEditorWindowWidget::refineBoundary(Map::Edge & e, TerrainLOD targetLOD) {
    // Figure out how many segments we need, based on the resolution for the
    // target height field LOD. We don't need any more segments than half the
    // number of elevation samples covering this length, since we can view the
    // elevation grid as sampling this edge, and it can't resolve any boundary
    // features with frequency higher than half its resolution (God bless you,
    // Mr. Nyquist!).
    //
    // NOTE: this is not exactly correct; the further the refinement is allowed
    // to vary from the original, the longer each segment is stretched. To
    // really handle this right, we ought to be calculating an expected curve
    // length based on the smoothness parameter (and others), and then getting
    // the number of segments from that.
    int segments = static_cast<int>(samplesPerMeterForLOD(targetLOD) * e.length());
    if (segments <  2) segments = 2;

    // Find the TerrainSeam for this edge, giving the edge shape paramters
    TerrainSeamConstPtr ts = e.terrainSeam();
    scalar_t smoothness = ts->smoothness();


    // Report what we're about to do to our oh-so-intelligent user
    // (HAIL TO THEE, USER!)
    INCA_INFO("Refining Edge  " << e.id() << '\n'
           << "   Smoothness:       " << smoothness << '\n'
           << "   Segments:         " << segments   << '\n')


    // Run the GA and get the strongest result
    _boundaryGA.run(segments, smoothness);
    IndexType idx = _boundaryGA.strongestChromosomeIndex();
    const BoundaryGA::Chromosome & c = _boundaryGA.chromosome(idx);
    
    // Decode it to cartesian coordinates
    Map::Point p(0);
    Map::PointList points;
    points.push_back(p);        // First point is at the origin
    BoundaryGA::Chromosome::const_iterator it;
    for (it = c.begin(); it != c.end(); ++it) {
        scalar_t angle = it->absoluteAngle;
        p[0] += cos(angle);     // Step by one unit in the indicated direction
        p[1] += sin(angle);
        points.push_back(p);
    }

    // Finally, set it as the boundary refinement for this edge
    e.addRefinement(points);
}


void MapEditorWindowWidget::generateHeightfieldForView() {
    // Create an IDMap of the appropriate size
    IDMap ids(renderer().viewport().bounds());

    // Render the map, sans decorations, with the terrain type as the color
    // HACK--this should create an off-screen context, sized appropriately
    // for the rendered region at its LOD. Right now, just using the screen
    // view as the rendered area
    _mapView->object()->renderFacesAsTerrainTypeIDs(true);
    GL::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::glBegin(GL_QUADS);
        GL::glColor3ub(0, 0, 0x02);
        GL::glVertex2i(-1000, -1000);
        GL::glVertex2i(1000, -1000);
        GL::glVertex2i(1000, 1000);
        GL::glVertex2i(-1000, 1000);
    GL::glEnd();
    render();
    _mapView->object()->renderFacesAsTerrainTypeIDs(false);
    
    // Copy the framebuffer into it
    GL::glReadBuffer(GL_BACK);
    GL::glReadPixels(ids.base(0), ids.base(1),
                     ids.size(0), ids.size(1),
                     GL_RGBA, GL_UNSIGNED_BYTE, ids.elements());

    // Decode "colors" back to terrain types
    for (IndexType y = ids.base(1); y <= ids.extent(1); ++y)
        for (IndexType x = ids.base(0); x <= ids.extent(0); ++x) {
            IDType id = ids(x, y);
            ids(x, y) = (id & 0xFF0000) >> 16
                      | (id & 0x00FF00)
                      | (id & 0x0000FF) << 16;
        }

    TerrainosaurusApplication & app = TerrainosaurusApplication::instance();
#if 1
    // Generate a base terrain from it
    MapRasterizationPtr mr(new MapRasterization(ids, LOD_30m,
                                                _map->terrainLibrary()));
    app.createTerrainSampleWindow(mr);
#else
    // Display it as greyscale
    Heightfield tmp(ids.bounds());
    for (IndexType y = ids.base(1); y <= ids.extent(1); ++y)
        for (IndexType x = ids.base(0); x <= ids.extent(0); ++x)
            tmp(x, y) = ids(x, y);
    app.createImageWindow(tmp);
#endif
}


// Event handlers
void MapEditorWindowWidget::keyPressed(const KeyEvent & e) {
    static float newSmooth = 0.2f;
    switch (e.keyCode()) {
        case KeyF1:
            _mapView->object()->toggle("Vertices");
            requestRedisplay();
            break;
        case KeyF2:
            _mapView->object()->toggle("Edges");
            requestRedisplay();
            break;
        case KeyF3:
            _mapView->object()->toggle("Faces");
            requestRedisplay();
            break;
        case KeyF4:
            _mapView->object()->toggle("Refinements");
            requestRedisplay();
            break;
        case KeyF5:
            _mapView->object()->toggle("Tangents");
            requestRedisplay();
            break;
        case KeyF6:
            _mapView->object()->toggle("Envelopes");
            requestRedisplay();
            break;
        case KeyA:
            if (e.modifierKeyActive(ControlModifierKey)) {
                persistentSelection()->selectAll();
                requestRedisplay();
            } else {
                WindowControlWidget::keyPressed(e);
            }
            break;
        case KeyTab: {
            MeshSelection::ElementType newType;
            switch (persistentSelection()->elementType()) {
                case MeshSelection::Edges:
                    newType = MeshSelection::Faces;
                    break;
                case MeshSelection::Faces:
                    newType = MeshSelection::Vertices;
                    break;
                case MeshSelection::Vertices:
                    newType = MeshSelection::Edges;
                    break;
            }
            _persistentSelection->setElementType(newType);
            _transientSelection->setElementType(newType);
            requestRedisplay();
            break;
        }
        case KeyZ:
            fitMap();
            break;
        case KeyG:
            generateHeightfieldForView();
            break;
        case KeySpace:
            refineBoundaries(*persistentSelection(), LOD_30m);
            requestRedisplay();
            break;
        case Key1:
            refineBoundary(*_map->edge(0), LOD_30m);
            requestRedisplay();
            break;
        case KeyUp:
            newSmooth += 0.1f;
            if (newSmooth > 1.0f) newSmooth = 1.0f;
            for (int i = 0; i < _map->terrainLibrary()->size(); ++i)
                for (int j = i + 1; j < _map->terrainLibrary()->size(); ++j)
                    _map->terrainLibrary()->terrainSeam(i, j)->smoothness = newSmooth;
            refineMap(LOD_30m);
            requestRedisplay();
            break;
        case KeyDown:
            newSmooth -= 0.1f;
            if (newSmooth < 0.0f) newSmooth = 0.0f;
            for (int i = 0; i < _map->terrainLibrary()->size(); ++i)
                for (int j = i + 1; j < _map->terrainLibrary()->size(); ++j)
                    _map->terrainLibrary()->terrainSeam(i, j)->smoothness = newSmooth;
            refineMap(LOD_30m);
            requestRedisplay();
            break;
        default:
            WindowControlWidget::keyPressed(e);
            break;
    }
}

void MapEditorWindowWidget::fitMap() {
    // Sanity checks!
    if (! _map) {
        INCA_WARNING(__FUNCTION__ << "() called, but window has no map")
        return;
    } else if (_map->vertexCount() == 0) {
        INCA_WARNING(__FUNCTION__ << "() called, but map has no vertices")
        return;
    }
    
    // XXX Perhaps this whole operation could be folded into Region?
    // Maybe something like r.engulfPoint(P), r.engulfPoints(vector)

    // Run over the whole map and find the X,Y extrema
    Map::vertex_const_iterator vi = _map->vertices().begin();
    Map::Point minP((*vi)->position()), maxP(minP);
    while (++vi != _map->vertices().end()) {
        Map::Point p = (*vi)->position();
        for (IndexType i = 0; i < 2; ++i)
            if (p[i] < minP[i])         minP[i] = p[i];
            else if (p[i] > maxP[i])    maxP[i] = p[i];
    }
    
    // Find the world-space center point
    Map::Point center((minP[0] + maxP[0]) / 2, (minP[1] + maxP[1]) / 2);
    
    // Find the screen area we have, and a smaller region inside this, leaving
    // a fixed number of pixels' padding around the map (the 'active' area)
    Dimension windowSize = size();
    Dimension activeSize = windowSize - Dimension(FIT_PADDING * 2);

    // Find the scale factor for the camera to make it all fit
    Map::Vector worldDimensions = maxP - minP;
    scalar_t unitsPerPixel = std::max(worldDimensions[0] / activeSize[0],
                                      worldDimensions[1] / activeSize[1]);
    Map::Vector cameraShape(windowSize[0] * unitsPerPixel,
                            windowSize[1] * unitsPerPixel);

    OrthographicCameraPtr camera =
        static_pointer_cast<OrthographicCamera>(_navigation->camera());
    camera->transform()->position = Transform::Point(center[0], center[1],
                                                     CAMERA_POSITION[2]);
    camera->viewWidth  = cameraShape[0];
    camera->viewHeight = cameraShape[1];
    
    requestRedisplay();

    INCA_DEBUG(__FUNCTION__ << "(): Fitting to region " << minP << " => " << maxP)
    INCA_DEBUG("     Size is " << worldDimensions)
    INCA_DEBUG("     Center is " << center)
    INCA_DEBUG("     Window size is " << windowSize)
    INCA_DEBUG("     Active size is " << activeSize)
    INCA_DEBUG("     Units per pixel are " << unitsPerPixel)
    INCA_DEBUG("     Camera shape is " << cameraShape)
}


// Constructor
MapEditorWindow::MapEditorWindow(MapPtr m, const std::string & nm)
    : Window(nm), _map(m),
      _persistentSelection(new MeshSelection()),
      _transientSelection(new MeshSelection()) { }

MapEditorWindow::MapEditorWindow(MapPtr m,
                                 MeshSelectionPtr ps,
                                 MeshSelectionPtr ts,
                                 const std::string & nm)
    : Window(nm), _map(m),
      _persistentSelection(ps),
      _transientSelection(ts) { }

void MapEditorWindow::construct() {
    // HACK : call superclass version...need other mechanism
    Window::construct();

    // Set up the window
    setSize(WINDOW_SIZE);
    setFullScreen(FULL_SCREEN);

    // Attach the widget chain
    MapEditorWindowWidgetPtr w(new MapEditorWindowWidget("Map Editor Window"));
    surface()->setWidget(w);
    
    // Make sure the MeshSelections are linked to the map
    _persistentSelection->setMap(_map);
    _transientSelection->setMap(_map);
    
    // Propagate data objects to the widget
    setMap(_map);
    setPersistentSelection(_persistentSelection);
    setTransientSelection(_transientSelection);
}


// Map accessors
MapPtr      MapEditorWindow::map()       { return _map; }
MapConstPtr MapEditorWindow::map() const { return _map; }
void MapEditorWindow::setMap(MapPtr m)   {
    _map = m;
    MapEditorWindowWidgetPtr w
        = static_pointer_cast<MapEditorWindowWidget>(surface()->widget());
    if (w) w->setMap(m);
}

// Persistent selection accessors
MeshSelectionPtr      MapEditorWindow::persistentSelection() {
    return _persistentSelection;
}
MeshSelectionConstPtr MapEditorWindow::persistentSelection() const {
    return _persistentSelection;
}
void MapEditorWindow::setPersistentSelection(MeshSelectionPtr s) {
    _persistentSelection = s;
    MapEditorWindowWidgetPtr w
        = static_pointer_cast<MapEditorWindowWidget>(surface()->widget());
    if (w) w->setPersistentSelection(s);
}

// Transient selection accessors
MeshSelectionPtr      MapEditorWindow::transientSelection() {
    return _transientSelection;
}
MeshSelectionConstPtr MapEditorWindow::transientSelection() const {
    return _transientSelection;
}
void MapEditorWindow::setTransientSelection(MeshSelectionPtr s) {
    _transientSelection = s;
    MapEditorWindowWidgetPtr w
        = static_pointer_cast<MapEditorWindowWidget>(surface()->widget());
    if (w) w->setTransientSelection(s);
}
