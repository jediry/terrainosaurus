/*
 * File: MapExplorerWidget.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "MapExplorerWidget.hpp"
using namespace terrainosaurus;

// Import other parts of the Inca UI framework
#include <inca/ui/Application.hpp>
#include <inca/ui/CompositeWidget.hpp>
using namespace inca::rendering;
using namespace inca::ui;


/*---------------------------------------------------------------------------*
 | Constructor
 *---------------------------------------------------------------------------*/
MapExplorerWidget::MapExplorerWidget(const string &nm)
        : PassThruWidget(nm), map(this),
          backgroundColor(this) {

    ///////////////////////////////////////////////////////////////////////////
    // Step 1: construct shared global objects
    camera2D = OrthographicCameraPtr(new OrthographicCamera());
        camera2D->viewWidth = 15.0;
        camera2D->viewHeight = 15.0;
        camera2D->nearClip = -1.0;
        camera2D->transform->position = Transform::Point(0.0, 0.0, 100.0);
    //PerspectiveCameraPtr  camera3D(new PerspectiveCamera());


    ///////////////////////////////////////////////////////////////////////////
    // Step 2: build the widget stack

    // The MultiplexorWidget chooses between major application modes
    modeSelect = MultiplexorWidgetPtr(new MultiplexorWidget("Mode Select"));
        widget = modeSelect;    // This is our highest-level subwidget

    // The 2D map editing mode widget stack
    CompositeWidgetPtr modeEdit2D(new CompositeWidget("2D Edit Mode"));
    navigate2D   = CameraControlPtr(new CameraControl("2D Navigation"));
    toolSelect2D = MultiplexorWidgetPtr(new MultiplexorWidget("2D Tool Select"));
    MapEditWidgetPtr translate2D (new TranslateWidget("2D Translate"));
    MapEditWidgetPtr rotate2D    (new RotateWidget("2D Rotate"));
    MapEditWidgetPtr scale2D     (new ScaleWidget("2D Scale"));
    MapEditWidgetPtr topology2D  (new TopologyWidget("Map topology"));
    viewSelect2D = MapSelectWidgetPtr(new MapSelectWidget("2D Map View/Select"));
        modeSelect->addWidget(modeEdit2D);
            modeEdit2D->control = navigate2D;
                navigate2D->control = toolSelect2D;
                navigate2D->camera = camera2D;  // Control the 2D camera
                navigate2D->enableLook = false;     // Disable capabilities
                navigate2D->enableDolly = false;    // that don't make sense
                navigate2D->enableYaw = false;      // in two dimensions
                navigate2D->enablePitch = false;
                navigate2D->panScale = 0.1;         // Tweak the controls
                navigate2D->rollScale = 3.1415962 / 150.0;
                navigate2D->zoomScale = 1.001;
            modeEdit2D->view = toolSelect2D;
                tools2D.push_back(translate2D);
                tools2D.push_back(rotate2D);
                tools2D.push_back(scale2D);
                tools2D.push_back(topology2D);
                for (index_t i = 0; i < index_t(tools2D.size()); i++) {
                    toolSelect2D->addWidget(tools2D[i]);// Add to chooser
                    tools2D[i]->widget = viewSelect2D;  // Link to next widget
                    tools2D[i]->selection = viewSelect2D->selection();
                }
                
    // Create an empty map
    map = MapPtr(new Map());
    
    // Start with the draw tool
    toolSelect2D->selectWidget("2D Scale");
    loadMap("blah blah");
    loadParams("genetic_params.txt");
}

// Custom setter for the "map" property
void MapExplorerWidget::ptr_property_set(Map, map) {
    _map = value;   // Set the value on this object

    // Set it for each tool widget
    for (index_t i = 0; i < index_t(tools2D.size()); i++)
        tools2D[i]->map = value;

    // Set it for the selection/rendering widget
    viewSelect2D->map = value;
}

// Custom setter for the "backgroundColor" property
void MapExplorerWidget::property_set(Color, backgroundColor) {
    _backgroundColor = value;
    renderer.setBackgroundColor(_backgroundColor);
}


/*---------------------------------------------------------------------------*
 | Widget event-handler functions
 *---------------------------------------------------------------------------*/
// Ensure the background color gets set the first time
void MapExplorerWidget::initializeView() {
    renderer.setBackgroundColor(backgroundColor());
}

// Handle all of the icky details of resizing
void MapExplorerWidget::resizeView(Dimension sz) {
    if (sz != size) {   // Don't do anything if we're already the right size
        PassThruWidget::resizeView(sz);     // Do superclass impl.
        renderer.resize(sz[0], sz[1]);      // Reshape the viewport
        camera2D->reshape(sz[0], sz[1]);    // Reshape the camera
    }
}

// Set up for rendering and render child widgets
void MapExplorerWidget::renderView() {
    // First, clear the display
    renderer.clear();

    // Next, set up the camera projection (could have changed!)
    renderer.resetProjectionMatrix();
    renderer.applyCameraProjection(camera2D);
    
    // Now, set up the camera transformation on behalf of the sub-widgets
    renderer.resetTransformMatrix();
    renderer.applyCameraTransform(camera2D);

    // Last, render the sub-widgets
    PassThruWidget::renderView();
}

void MapExplorerWidget::keyPressed(KeyCode key, Pixel p) {
    switch (key) {
        // Alt-Q == quit
        case KeyQ:
            if (theseModifiersActive(AltModifier)) {
                Application::instance().exit(0, "Exiting normally...");
                return; // Of course, we never actually get here...
            }
            break;

        // F11 == toggle full-screen mode
        case KeyF11:
            if (theseModifiersActive(NoModifiers)) {
                cerr << "Full-screen not implemented yet\n";
                return;
            }
            break;

        // Alt-L == Load parameters file
        case KeyL:
            if (theseModifiersActive(AltModifier)) {
                loadParams("genetic_params.txt");
                requestRedisplay();
                return;
            }
            break;

        // Alt-O == Open map file
        case KeyO:
            if (theseModifiersActive(AltModifier)) {
                loadMap("testmap.obj");
                requestRedisplay();
                return;
            }
            break;

        // Number keys switch between tools
        case Key1: case Key2: case Key3: case Key4: case Key5:
        case Key6: case Key7: case Key8: case Key9: case Key0:
            if (theseModifiersActive(NoModifiers)) {
                map->numberOfCycles = (key - Key1);
                return;
            }
            break;

        // Tab/Shift-Tab switches between tools
        case KeyTab:
            if (theseModifiersActive(NoModifiers)) {
                toolSelect2D->selectNextWidget();
                return;
            } else if (theseModifiersActive(ShiftModifier)) {
                toolSelect2D->selectPreviousWidget();
                return;
            }
            break;

    case KeyR:      // 'R' == Refine map
        if (theseModifiersActive(AltModifier)) {
            if (map)
                map->refineMap();
            requestRedisplay();
            return;
        }
        break;
    }
    
    // If we got here, this message isn't for us. Move along. Move along.
    PassThruWidget::keyPressed(key, p);
}


/*---------------------------------------------------------------------------*
 | I/O functions
 *---------------------------------------------------------------------------*/
void MapExplorerWidget::loadParams(const string &filename) {
    if (!map()) {
        cerr << "There is no map right now" << endl;
        return;
    }

    std::ifstream ifs;
    ifs.open(filename.c_str());
    if (!ifs) {
        cerr << "Failed to open " << filename << endl;
        return;
    }

    string name, assigned;
    float value;;
    while (! ifs.eof()) {
        ifs >> name >> assigned >> value;
        if (! ifs) {
            cerr << "Error reading from file" << endl;
            return;
        }
        if (name == "numberOfCycles")
            map->numberOfCycles = (int)value;
        else if (name == "numberOfChromosomes")
            map->numberOfChromosomes = (int)value;
        else if (name == "smoothness")
            map->smoothness = value;
        else if (name == "mutationRate")
            map->mutationRate = value;
        else if (name == "crossoverRate")
            map->crossoverRate = value;
        else if (name == "selectionRatio")
            map->selectionRatio = value;
        else if (name == "resolution")
            map->resolution = value;
        else {
            cerr << "Unrecognized parameter " << name << endl;
            return;
        }
        cerr << "Set " << name << " = " << value << endl;
    }
    if (!ifs.eof())
        ifs.clear();
}


void MapExplorerWidget::loadMap(const string &filename) {
    MapPtr newMap(new Map());

    Map::PolygonMeshPtr mesh = newMap->mesh();
    mesh->createVertex(Map::Point(0.0, 0.0));
    mesh->createVertex(Map::Point(1.5, -1.5));
    mesh->createVertex(Map::Point(3.0, 0.0));
    mesh->createVertex(Map::Point(1.5, 1.5));
    mesh->createVertex(Map::Point(-1.5, 1.5));
    mesh->createVertex(Map::Point(-3.0, 0.0));
    mesh->createVertex(Map::Point(-1.5, -1.5));
    Map::PolygonMesh::FaceVertexPtrList list;
    list.push_back(mesh->createFaceVertex(mesh->vertex(0)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(1)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(2)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(3)));
    Map::PolygonMesh::FacePtr f = mesh->createFace(list);
    f->setMaterial(0);
    list.clear();
    list.push_back(mesh->createFaceVertex(mesh->vertex(0)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(4)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(5)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(6)));
    f = mesh->createFace(list);
    f->setMaterial(1);

    // Create the necessary terrain types
    newMap->library.add(TerrainDescriptorPtr(new TerrainDescriptor()));
    newMap->library[0]->color = Color(0.0f, 0.5f, 0.2f, 1.0f);
    newMap->library.add(TerrainDescriptorPtr(new TerrainDescriptor()));
    newMap->library[1]->color = Color(0.5f, 0.5f, 0.0f, 1.0f);

    // Set this as our current map object (the custom setter will propagate
    // it to the sub-widgets)
    map = newMap;

#if 0
    Inca::IO::OBJInterpreter obj;
    ObjectPtr model;

    // Try to load the file    
    try {
        model = obj.loadModel(filename);
    } catch (Inca::IO::FileException &e) {
        cerr << "Uh oh! " << e.getMessage() << endl;
    }

    SolidObject3D * meshObj = (SolidObject3D *)model.get();
    if (model != NULL) {
        // Flatten the mesh into 2D and tell the map about it
        mapObject = SolidObject2DPtr(new SolidObject2D());
        mapObject->setTransform(TransformPtr(new Transform()));
        mapObject->transform()->setLocationPoint(new Transform::Point(0.0));
        mapObject->setTessellation(new Map::PolygonMesh(*meshObj->tessellation(), 2));
        _map->setMesh(mapObject->tessellation());
        
        // Create a GL proxy to render the mesh
        mapProxy = GLRenderer::createProxy(ObjectPtr(mapObject));
    }
#endif
}

void MapExplorerWidget::storeMap(const string &filename) const {

}
