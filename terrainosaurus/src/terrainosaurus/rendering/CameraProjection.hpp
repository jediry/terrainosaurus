/*
 * File: CameraProjection.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      FIXME: This class needs a different name. This is sorta just a hack until I figure out the "right" way to do this.
 */

#ifndef TERRAINOSAURUS_RENDERING_CAMERA_PROJECTION
#define TERRAINOSAURUS_RENDERING_CAMERA_PROJECTION

// Import Inca library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class CameraProjection;
};

// FIXME!!!
#include <inca/rendering.hpp>
// Renderer type
typedef inca::rendering::OpenGLRenderer Renderer;


class terrainosaurus::CameraProjection {
public:
    // Render function call operator
    void operator()(Renderer & renderer, const inca::world::PerspectiveCamera & camera) const {
        typedef Renderer::geometry_t geometry_t;
        Renderer::Matrix m(0);
        geometry_t f = inca::math::cot(camera.vertViewAngle() * geometry_t(0.5));
        geometry_t zNear = camera.nearClip(),
                   zFar  = camera.farClip(),
                   zDiff = zNear - zFar;
        m(0,0) = f / camera.aspectRatio();
        m(1,1) = f;
        m(2,2) = (zFar + zNear) / zDiff;
        m(2,3) = (2 * zFar * zNear) / zDiff;
        m(3,2) = geometry_t(-1);
        renderer.projectionMatrix().load(m);
    }
};

#endif
