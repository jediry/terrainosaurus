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
 */

// Import class definition
#include "MapEditorWidget.hpp"
using Terrainosaurus::MapEditorWidget;


// Windows OpenGL seems to need this
#if __MS_WINDOZE__
#   include <windows.h>
#endif

// Import OpenGL
#include <GL/gl.h>
#include <GL/glu.h>


/*---------------------------------------------------------------------------*
 | Constructor
 *---------------------------------------------------------------------------*/
MapEditorWidget::MapEditorWidget()
         : CameraControl(CameraPtr(new OrthographicCamera())), _map(new Map()) {
    // Use an orthographic view of the map
    TransformPtr xform(new Transform());
    xform->setLocationPoint(new Transform::Point(0.0, 0.0, 100.0));
    camera()->setTransform(xform);
    ((OrthographicCamera *)camera().get())->setViewWidth(5.0);
    ((OrthographicCamera *)camera().get())->setViewHeight(5.0);

    // Create a GL proxy for the camera
    cameraProxy = renderer.createProxy(camera());

    // Fine tune the GUI control parameters
    setDistanceScale(0.3);
    setAngleScale(Transform::PI / 100.0);
    setZoomScale(1.001);

    // Setup the default material
    MaterialPtr m(new Material());
    m->setColor(Material::Color(0.0f, 0.8f, 0.2f));
    m->setOpacity(0.75f);
    GLMaterialProxyPtr defMat(GLRenderer::createProxy(m));
    GLMaterialProxy::setDefaultMaterial(defMat);
}


/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
void MapEditorWidget::initializeView() { }

void MapEditorWidget::resizeView(unsigned int w, unsigned int h) {
    camera()->reshape(w, h);
    cameraProxy->setupProjection();
    glViewport(0, 0, w, h);
}

void MapEditorWidget::renderView() {
    renderer.initialize();
    renderer.clear();
    renderer.setRenderMode(Paint);
    cameraProxy->setupView();
    
    if (mapProxy != NULL) {
        renderer.pushTransform(mapObject->transform());
        // Render the mesh itself
        mapProxy->render(Opacity);
        
        // Now render any refined boundaries
        Map::BoundaryList &b = map()->boundaries();
        Map::BoundaryList::const_iterator it;
        for (it = b.begin(); it != b.end(); it++)
            if (it->isRefined()) {
                Map::RefinedBoundary rb = it->refinement();
                glColor3d(0.0, 0.0, 1.0);
                glBegin(GL_LINE_STRIP);
                    for (unsigned int i = 0; i < rb.size(); i++)
                        glVertex2dv(rb[i]);
                glEnd();
            }
        renderer.popTransform();
    }
}


/*---------------------------------------------------------------------------*
 | Event-handlers
 *---------------------------------------------------------------------------*/
void MapEditorWidget::mouseDragged(unsigned int x, unsigned int y) {
    // Figure out how far we've moved (flipping Y)
    int dx = x - mouseX,
        dy = mouseY - y;

    if (controlFlags == (CTRL_KEY | LEFT_BUTTON))
        zoomCamera(dx + dy);
    else if (controlFlags == (CTRL_KEY | MIDDLE_BUTTON))
        panCamera(dx, dy);
    else if (controlFlags == (CTRL_KEY | RIGHT_BUTTON))
        rollCamera(dx + dy);

    // Update our saved coordinates
    mouseX = x;
    mouseY = y;
}

void MapEditorWidget::mouseTracked(unsigned int x, unsigned int y) {
    // Defer to the superclass's implementation
    CameraControl::mouseTracked(x, y);
}

void MapEditorWidget::buttonPressed(MouseButton button,
                                    unsigned int x, unsigned int y) {
    // Defer to the superclass's implementation
    CameraControl::buttonPressed(button, x, y);
}

void MapEditorWidget::keyPressed(KeyCode k, unsigned int x, unsigned int y) {
    switch (k) {
    case KEY_Q:
        application->exit(0, "Exited normally");
        break;
    case KEY_PERIOD:
        mapObject->transform()->rotateZ(-Transform::PI / 6);
        requestRedisplay();
        break;
    case KEY_COMMA:
        mapObject->transform()->rotateZ(Transform::PI / 6);
        requestRedisplay();
        break;
    case KEY_O:
        loadMap("testmap.obj");
        requestRedisplay();
        break;
    case KEY_R:
        map()->refineMap();
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
}

void MapEditorWidget::storeMap(const string &filename) const {

}

