/*
 * File: MapEditorWidget.c++
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MapEditor class implements a toolkit-independent GUI widget for
 *      viewing and editing 2D maps made up of adjacent polygons. Each polygon
 *      may be dynamically assigned a terrain type. Any number of polygons may
 *      be drawn -- overlapping polygons of the same type will be merged, while
 *      overlapping polygons of different types will
 *      XXX
 */

// Import class definition
#include "MapEditorWidget.hpp"
using terrainosaurus::MapEditorWidget;


/*---------------------------------------------------------------------------*
 | Constructor
 *---------------------------------------------------------------------------*/
MapEditorWidget::MapEditorWidget()
         : CameraControl(CameraPtr(new OrthographicCamera())), map(this),

           // Rendering attributes
           terrainColors(this), backgroundColor(this), boundaryColor(this),
           refinedBoundaryColor(this), intersectionColor(this),
           gridColor(this), boundaryWidth(this), refinedBoundaryWidth(this),
           intersectionSize(this),

           // Rendering flags
           renderRegions(this), renderBoundaries(this),
           renderRefinedBoundaries(this), renderIntersections(this),
           renderGrid(this) {

    // Use an orthographic view of the map
    TransformPtr xform = camera()->transform();
    xform->position = Transform::Point(0.0, 0.0, 100.0);
    camera()->nearClip = -1.0;
    ((OrthographicCamera *)camera().get())->viewWidth = 5.0;
    ((OrthographicCamera *)camera().get())->viewHeight = 5.0;

    // Fine tune the GUI control parameters
    panScale = 0.1;
    rollScale = 3.1415962 / 150.0;
    zoomScale = 1.001;

    // Create a map (#$%@ idiot!)
    loadMap("");

    // Setup the default material
//    MaterialPtr m(new Material());
//    m->setColor(Material::Color(0.0f, 0.8f, 0.2f));
//    m->setOpacity(0.75f);
//    GLMaterialProxyPtr defMat(GLRenderer::createProxy(m));
//    GLMaterialProxy::setDefaultMaterial(defMat);
}


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
void MapEditorWidget::initializeView() {
    renderer.initialize();
    renderer.setBackgroundColor(backgroundColor());
    renderer.setShadingMode(Paint);
    renderer.enableLineSmoothing(true);
    renderer.enablePointSmoothing(true);
}

void MapEditorWidget::resizeView(size_t w, size_t h) {
    camera()->reshape(w, h);
    resetCameraProjection();
    renderer.resize(w, h);

    if (map()) {
        logger << "The map has:" << endl
               << "   " << map()->regionCount() << " regions" << endl
               << "   " << map()->intersectionCount() << " intersections" << endl
               << "   " << map()->boundaryCount() << " boundaries" << endl;
        logger.info();
    }
}

void MapEditorWidget::resetCameraProjection() {
    renderer.resetCameraProjection();
    renderer.applyCameraProjection(camera());
}

void MapEditorWidget::renderView() {
    renderer.clear();

    // Transform to the camera's location
    renderer.resetTransform();
    renderer.applyCameraTransform(camera());
    
    if (map()) {
        // Transform the map object
//        renderer.applyTransform(mapObject->transform());

        // Render the map regions 
        if (renderRegions) {
            const Map::RegionList &rs = map()->regions();
            Map::RegionList::const_iterator it;

            for (it = rs.begin(); it != rs.end(); it++) {
                Map::Region r = *it;
                //renderer.setColor(terrainColor[r.terrainType()]);
                renderer.beginRenderImmediate(Polygon);
                    for (index_t i = 0; i < index_t(r.intersectionCount()); i++)
                        renderer.renderVertex(r.intersection(i).location());
                renderer.endRenderImmediate();
            }
        }

        // Render the unrefined map boundaries
        if (renderBoundaries) {
            const Map::BoundaryList &bs = map()->boundaries();
            Map::BoundaryList::const_iterator it;
            renderer.setColor(boundaryColor());
            renderer.setLineWidth(boundaryWidth());

            renderer.beginRenderImmediate(Lines);
            for (it = bs.begin(); it != bs.end(); it++) {
                Map::Boundary b = *it;
                renderer.renderVertex(b.startIntersection().location());
                renderer.renderVertex(b.endIntersection().location());
            }
            renderer.endRenderImmediate();
        }

        // Render the refined boundaries
        if (renderRefinedBoundaries) {
            const Map::BoundaryList &bs = map()->boundaries();
            Map::BoundaryList::const_iterator it;
            renderer.setColor(refinedBoundaryColor());
            renderer.setLineWidth(refinedBoundaryWidth());

            for (it = bs.begin(); it != bs.end(); it++) {
                Map::RefinedBoundary rb = it->refinement();
                renderer.beginRenderImmediate(LineStrip);
                    for (index_t i = 0; i < index_t(rb.size()); i++)
                        renderer.renderVertex(rb[i]);
                renderer.endRenderImmediate();
            }
        }

        // Render the map boundary intersections
        if (renderIntersections) {
            const Map::IntersectionList &is = map()->intersections();
            Map::IntersectionList::const_iterator it;
            renderer.setColor(intersectionColor());
            renderer.setPointSize(intersectionSize());

            renderer.beginRenderImmediate(Points);
            for (it = is.begin(); it != is.end(); it++) {
                Map::Intersection i = *it;
                renderer.renderVertex(i.location());
            }
            renderer.endRenderImmediate();
        }

        // Render a regular grid over the window
        if (renderGrid) {
            Point ps, pe;
            renderer.setColor(gridColor());
            renderer.beginRenderImmediate(Lines);

            // Draw horizontal grid lines
            
            // Draw vertical grid lines

            renderer.endRenderImmediate();
        }
    }
}


/*---------------------------------------------------------------------------*
 | Event-handlers
 *---------------------------------------------------------------------------*/
void MapEditorWidget::mouseDragged(index_t x, index_t y) {
    // Figure out how far we've moved (flipping Y)
    int dx = x - mouseX,
        dy = mouseY - y;

    if (controlFlags == (CTRL_KEY | LEFT_BUTTON)) {
        zoomCamera(dx + dy);
        resetCameraProjection();
    } else if (controlFlags == (CTRL_KEY | MIDDLE_BUTTON))
        panCamera(dx, dy);
    else if (controlFlags == (CTRL_KEY | RIGHT_BUTTON))
        rollCamera(dx + dy);

    // Update our saved coordinates
    mouseX = x;
    mouseY = y;
}

void MapEditorWidget::mouseTracked(index_t x, index_t y) {
    // Defer to the superclass's implementation
    CameraControl::mouseTracked(x, y);
}

void MapEditorWidget::buttonPressed(MouseButton button,
                                    index_t x, index_t y) {
    // Defer to the superclass's implementation
    CameraControl::buttonPressed(button, x, y);
}

void MapEditorWidget::keyPressed(KeyCode k, index_t x, index_t y) {
    switch (k) {
    case KEY_ESCAPE:
        Application::instance().exit(0, "Exited normally");
        break;
    case KEY_F1:    renderRegions           = !renderRegions();         break;
    case KEY_F2:    renderIntersections     = !renderIntersections();   break;
    case KEY_F3:    renderBoundaries        = !renderBoundaries();      break;
    case KEY_F4:    renderRefinedBoundaries = !renderRefinedBoundaries(); break;
    case KEY_F5:    renderGrid              = !renderGrid();            break;
    case KEY_F11:   container()->setFullScreen(true);                   break;
    case KEY_LESS_THAN:
//        mapObject->transform()->rotateZ(-Transform::PI / 6);
        requestRedisplay();
        break;
    case KEY_GREATER_THAN:
//        mapObject->transform()->rotateZ(Transform::PI / 6);
        requestRedisplay();
        break;
    case KEY_O:
        loadMap("testmap.obj");
        requestRedisplay();
        break;
    case KEY_R:
        map->refineMap();
        requestRedisplay();
        break;
    default:
        // Defer to the superclass's implementation
        CameraControl::keyPressed(k, x, y);
        break;
    }
}


/*---------------------------------------------------------------------------*
 | Map I/O functions
 *---------------------------------------------------------------------------*/
void MapEditorWidget::loadMap(const string &filename) {
    map = new Map();

    Map::PolygonMeshPtr mesh = map->mesh();
    mesh->createVertex(Point(0.0, 0.0));
    mesh->createVertex(Point(1.0, 0.0));
    mesh->createVertex(Point(1.0, 1.0));
    mesh->createVertex(Point(0.0, 1.0));
    mesh->createVertex(Point(0.5, 0.5));
    Map::PolygonMesh::FaceVertexPtrList list;
    list.push_back(mesh->createFaceVertex(mesh->vertex(0)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(1)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(4)));
    mesh->createFace(list);
    list.clear();
    list.push_back(mesh->createFaceVertex(mesh->vertex(2)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(3)));
    list.push_back(mesh->createFaceVertex(mesh->vertex(4)));
    mesh->createFace(list);


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

void MapEditorWidget::storeMap(const string &filename) const {

}

