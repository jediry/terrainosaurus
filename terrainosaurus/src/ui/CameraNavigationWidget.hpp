/*
 * File: CameraNavigationWidget.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

#ifndef TERRAINOSAURUS_CAMERA_NAVIGATION_WIDGET
#define TERRAINOSAURUS_CAMERA_NAVIGATION_WIDGET

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class CameraNavigationWidget;
    
    // Pointer typedefs
    typedef shared_ptr<CameraNavigationWidget>       CameraNavigationWidgetPtr;
    typedef shared_ptr<CameraNavigationWidget const> CameraNavigationWidgetConstPtr;
};

// Import superclass definition
#include <inca/ui/PassThruWidget.hpp>

// Import Camera & Transform definitions
#include <inca/world/Cameras.hpp>

// Import terrain-specialized renderer definition
#include "../TerrainosaurusRenderer.hpp"


class terrainosaurus::CameraNavigationWidget : public inca::ui::PassThruWidget {
private:
    // Set up this class to own properties
    PROPERTY_OWNING_OBJECT(CameraNavigationWidget);

    // Math typedefs
    typedef inca::world::Camera                     Camera;
    typedef inca::world::CameraPtr                  CameraPtr;
    typedef inca::world::Transform                  Transform;
    typedef inca::world::TransformPtr               TransformPtr;
    typedef inca::world::Transform::scalar_t        scalar_t;
    typedef inca::world::Transform::scalar_arg_t    scalar_arg_t;


/*---------------------------------------------------------------------------*
 | Constructors & Properties
 *---------------------------------------------------------------------------*/
public:
    // Default constructor with optional component name
    explicit CameraNavigationWidget(const string &nm = "");

    // Constructor initializing camera
    explicit CameraNavigationWidget(CameraPtr c, const string &nm = "");

    // The camera we're controlling
    rw_ptr_property(Camera, camera, NULL);

    // Transformation scale factors
    rw_property(scalar_t, dollyScale, 1.0);
    rw_property(scalar_t, lookScale, inca::math::PI<scalar_t>() / 64.0);
    rw_property(scalar_t, rollScale, inca::math::PI<scalar_t>() / 64.0);
    rw_property(scalar_t, pitchScale, inca::math::PI<scalar_t>() / 64.0);
    rw_property(scalar_t, yawScale, inca::math::PI<scalar_t>() / 64.0);
    rw_property(scalar_t, zoomScale, 1.01);

    // Transformation enabled toggles
    rw_property(bool, enablePan,   true);
    rw_property(bool, enableDolly, true);
    rw_property(bool, enableLook,  true);
    rw_property(bool, enableRoll,  true);
    rw_property(bool, enablePitch, true);
    rw_property(bool, enableYaw,   true);
    rw_property(bool, enableZoom,  true);
    
    // The renderer
    rw_ptr_property(TerrainosaurusRenderer, renderer, NULL);


/*---------------------------------------------------------------------------*
 | Event-handling functions
 *---------------------------------------------------------------------------*/
public:
    void resizeView(Dimension d);
    void renderView();

    void keyPressed(inca::ui::KeyCode keycode, Pixel p);
    void mouseTracked(Pixel p);
    void mouseDragged(Pixel p);
    void buttonPressed(inca::ui::MouseButton button, Pixel p);
    void buttonReleased(inca::ui::MouseButton button, Pixel p);
    void buttonClicked(inca::ui::MouseButton b, Pixel p);

protected:
    void pushMatrices();
    void popMatrices();

    Pixel mousePosition;    // Last recorded mouse coordinates
    bool transforming;


/*---------------------------------------------------------------------------*
 | High-level camera controls (values correspond to pixels)
 *---------------------------------------------------------------------------*/
public:
    void panCamera(int dx, int dy);
    void dollyCamera(int dz);
    void lookCamera(int ay, int ap);
    void rollCamera(int ar);
    void pitchCamera(int ap);
    void yawCamera(int ay);
    void zoomCamera(int clicks);
};

#endif