/*
 * File: CameraNavigationWidget.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      This file implements the generic camera control declared in
 *      CameraNavigationWidget.hpp.
 */

// Import class definition
#include "CameraNavigationWidget.hpp"
using namespace terrainosaurus;
using namespace inca::world;
using namespace inca::ui;


// Default constructor with optional component name
CameraNavigationWidget::CameraNavigationWidget(const string &nm)
    : PassThruWidget(nm), camera(this), renderer(this),
      dollyScale(this), lookScale(this), zoomScale(this),
      yawScale(this), pitchScale(this), rollScale(this),
      enablePan(this), enableDolly(this), enableLook(this), enableRoll(this),
      enablePitch(this), enableYaw(this), enableZoom(this) { }

// Constructor initializing camera
CameraNavigationWidget::CameraNavigationWidget(CameraPtr c, const string &nm)
    : PassThruWidget(nm), camera(this, c), renderer(this),
      dollyScale(this), lookScale(this), zoomScale(this),
      yawScale(this), pitchScale(this), rollScale(this),
      enablePan(this), enableDolly(this), enableLook(this), enableRoll(this),
      enablePitch(this), enableYaw(this), enableZoom(this) { }


// Widget event handlers
void CameraNavigationWidget::resizeView(Dimension sz) {
    if (sz != size) {   // Don't do anything if we're already the right size
        renderer->resize(sz[0], sz[1]);     // Reshape the viewport
        camera->reshape(sz[0], sz[1]);      // Reshape the camera
        PassThruWidget::resizeView(sz);     // Do superclass impl.
    }

    // Call superclass implementation (pass on to sub-widget)
    PassThruWidget::resizeView(sz);
}

void CameraNavigationWidget::renderView() {
    // Set up the camera transformation on behalf of the sub-widgets
    pushMatrices();

        // Call superclass implementation (pass on to sub-widget)
        PassThruWidget::renderView();

    // Clean up the transformation stack
    popMatrices();
}

void CameraNavigationWidget::keyPressed(KeyCode keycode, Pixel p) {
    // Set up the camera transformation on behalf of the sub-widgets
    pushMatrices();

        // This event doesn't concern us, so just pass it along
        PassThruWidget::keyPressed(keycode, p);

    // Clean up the transformation stack
    popMatrices();
}

void CameraNavigationWidget::mouseTracked(Pixel p) {
    // Set up the camera transformation on behalf of the sub-widgets
    pushMatrices();

        // This event doesn't concern us, so just pass it along
        PassThruWidget::mouseTracked(p);

    // Clean up the transformation stack
    popMatrices();
}

void CameraNavigationWidget::mouseDragged(Pixel p) {
    // Set up the camera transformation on behalf of the sub-widgets
    pushMatrices();

        bool transforming = false;  // Set to true if we're doin' stuff

        // Figure out how far we've moved (flipping Y)
        int dx = p[0] - mousePosition[0],
            dy = mousePosition[1] - p[1];

        // Update our saved coordinates
        mousePosition = p;

        // If this event belongs to us, Autobots TRANSFORM!!!
        if (theseFlagsActive(ControlModifier | LeftButton)) {
            zoomCamera(dx + dy);
            transforming = true;
        } else if (theseFlagsActive(ControlModifier | AltModifier | LeftButton)) {
            dollyCamera(dx + dy);
            transforming = true;
        } else if (theseFlagsActive(ControlModifier | MiddleButton)) {
            panCamera(dx, dy);
            transforming = true;
        } else if (theseFlagsActive(ControlModifier | RightButton)) {
            rollCamera(dx + dy);
            transforming = true;
        }

        // If this event didn't concern us, just pass it along
        if (! transforming)
            PassThruWidget::mouseDragged(p);

    // Clean up the transformation stack
    popMatrices();
}

void CameraNavigationWidget::buttonPressed(MouseButton button, Pixel p) {
    // Set up the camera transformation on behalf of the sub-widgets
    pushMatrices();

        // Store these as the current mouse coordinates
        mousePosition = p;

        // Let the superclass do its thing
        PassThruWidget::buttonPressed(button, p);

    // Clean up the transformation stack
    popMatrices();
}

void CameraNavigationWidget::buttonReleased(MouseButton button, Pixel p) {
    // Set up the camera transformation on behalf of the sub-widgets
    pushMatrices();

        // Store these as the current mouse coordinates
        mousePosition = p;

        // Let the superclass do its thing
        PassThruWidget::buttonReleased(button, p);

    // Clean up the transformation stack
    popMatrices();
}

void CameraNavigationWidget::buttonClicked(MouseButton button, Pixel p) {
    // Set up the camera transformation on behalf of the sub-widgets
    pushMatrices();

        // Let the superclass do its thing
        PassThruWidget::buttonClicked(button, p);

    // Clean up the transformation stack
    popMatrices();
}


/*---------------------------------------------------------------------------*
 | High-level camera controls (values correspond to pixels)
 *---------------------------------------------------------------------------*/
void CameraNavigationWidget::pushMatrices() {
    // Push the camera's projection matrix onto the projection stack
    TerrainosaurusRenderer::api::select_projection_matrix();
    TerrainosaurusRenderer::api::push_matrix();
    TerrainosaurusRenderer::api::reset_matrix();
    renderer->applyCameraProjection(camera);

    // Push the camera's transformation onto the view stack
    TerrainosaurusRenderer::api::select_transformation_matrix();
    TerrainosaurusRenderer::api::push_matrix();
    TerrainosaurusRenderer::api::reset_matrix();
    renderer->applyCameraTransform(camera);
}

void CameraNavigationWidget::popMatrices() {
    TerrainosaurusRenderer::api::select_projection_matrix();
    TerrainosaurusRenderer::api::pop_matrix();
    TerrainosaurusRenderer::api::select_transformation_matrix();
    TerrainosaurusRenderer::api::pop_matrix();
}

void CameraNavigationWidget::panCamera(int dx, int dy) {
    typedef TerrainosaurusRenderer::Point3D Point3D;    // Shortcut

    if (camera && enablePan) {
        // Unproject 3 points one pixel apart onto the image plane (z = 1.0)
        Point3D pixel(size[0] / 2, size[1] / 2, 1.0);
        Point3D po = renderer->unproject(pixel);
        ++(pixel[0]);
        Point3D px = renderer->unproject(pixel);
        --(pixel[0]); ++(pixel[1]);
        Point3D py = renderer->unproject(pixel);

        // Calculate world-space distance traveled in X and Y directions
        camera->transform->pan(-dx * distance(po, px),
                               -dy * distance(po, py));

        // ...and redraw.
        requestRedisplay();
    }
}

void CameraNavigationWidget::dollyCamera(int dz) {
    if (camera && enableDolly) {
        camera->transform->dolly(dz * dollyScale());
        requestRedisplay();
    }
}

void CameraNavigationWidget::lookCamera(int ay, int ap) {
    if (camera && enableLook) {
        camera->transform->look(-ay * lookScale(), -ap * lookScale());
        requestRedisplay();
    }
}

void CameraNavigationWidget::rollCamera(int ar) {
    if (camera && enableRoll) {
        camera->transform->roll(ar * rollScale());
        requestRedisplay();
    }
}

void CameraNavigationWidget::pitchCamera(int ap) {
    if (camera && enablePitch) {
        camera->transform->pitch(ap * pitchScale());
        requestRedisplay();
    }
}

void CameraNavigationWidget::yawCamera(int ay) {
    if (camera && enableYaw) {
        camera->transform->yaw(ay * yawScale());
        requestRedisplay();
    }
}

void CameraNavigationWidget::zoomCamera(int clicks) {
    if (camera && enableZoom) {
        if (clicks > 0)
            for (int i = 0; i < clicks; i++)
                camera->zoom(zoomScale());
        else if (clicks < 0)
            for (int i = 0; i > clicks; i--)
                camera->zoom(1.0 / zoomScale());
 
        // ...and redraw
        requestRedisplay();
    }
}
