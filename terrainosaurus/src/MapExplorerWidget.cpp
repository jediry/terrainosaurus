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

// Import Inca Application definition
#include <inca/ui/Application.hpp>

using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;


/*---------------------------------------------------------------------------*
 | Constructor
 *---------------------------------------------------------------------------*/
MapExplorerWidget::MapExplorerWidget(const string &nm)
        : PassThruWidget(nm), map(this, new Map()) {

    ///////////////////////////////////////////////////////////////////////////
    // Step 1: construct shared global objects
    MapSelectionPtr selection(new MapSelection());
    OrthographicCameraPtr camera2D(new OrthographicCamera());
        camera2D->viewWidth = 5.0;
        camera2D->viewHeight = 5.0;
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
    select2D     = MapSelectWidgetPtr(new MapSelectWidget("2D Selection"));
    mapView2D    = MapViewPtr(new MapView("2D Map View"));
        modeSelect->addWidget(modeEdit2D);
            modeEdit2D->control = navigate2D;
                navigate2D->control = toolSelect2D;
                navigate2D->camera = camera2D;
                navigate2D->enableLook = false; // Disable most rotations
                navigate2D->panScale = 0.1;
                navigate2D->rollScale = 3.1415962 / 150.0;
                navigate2D->zoomScale = 1.001;
            modeEdit2D->view = toolSelect2D;
                tools2D.push_back(translate2D);
                tools2D.push_back(rotate2D);
                tools2D.push_back(scale2D);
                tools2D.push_back(topology2D);
                for (index_t i = 0; i < tools2D.size(); i++) {
                    toolSelect2D->addWidget(tools2D[i]);    // Add to chooser
                    tools2D[i]->widget = select2D;          // Link to select
                    tools2D[i]->selection = selection;      // Give selection
                }
                    select2D->view = mapView2D;
                        mapView2D->camera = camera2D;
                    select2D->selection = selection;
}

// Custom setter for the "map" property
void MapExplorerWidget::ptr_property_set(Map, map) {
    _map = value;   // Set the value on this object
    
    // Set it for each tool widget
    for (index_t i = 0; i < tools2D.size(); i++)
        tools2D[i]->map = value;

    // Set it for the selection and the rendering widgets
//    select2D->map = value;
    mapView2D->map = value;
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
    mesh->createFace(list);
    list.clear();
    list.push_back(mesh->createFaceVertex(mesh->vertex(0)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(4)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(5)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(6)));
    mesh->createFace(list);

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


/*---------------------------------------------------------------------------*
 | Widget event-handler functions
 *---------------------------------------------------------------------------*/
void MapExplorerWidget::renderView() {
    PassThruWidget::renderView();
}

void MapExplorerWidget::keyPressed(KeyCode key, Point p) {
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
//        case Key1: case Key2: case Key3: case Key4: case Key5:
//        case Key6: case Key7: case Key8: case Key9: case Key0:
//            if (theseModifiersActive(NoModifiers)) {
//                toolSet->setActiveWidget(key - Key1);
//                return;
//            }
//            break;

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

//    case KeyR:      // 'R' == Refine map
//        if (allModifiersActive(NoModifiers)) {
//            if (map)
//                map->refineMap();
//            requestRedisplay();
//            return;
//        }
//        break;
    }
    
    // If we got here, this message isn't for us. Move along. Move along.
    PassThruWidget::keyPressed(key, p);
}

