/*
 * File: SkyBox.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import class definition
#include "SkyBox.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;

namespace terrainosaurus {
    typedef OpenGLRenderer          Renderer;
    typedef Renderer::Vector3D      Vector3D;
    typedef Renderer::geometry_t    geometry_t;
}
#include <inca/integration/opengl/GL.hpp>
#include <FreeImagePlus.h>

// Features that we support

// HACK: things we can't configure
#define SKY_COLOR   Color(0.0f, 0.8f, 0.8f, 1.0f)
#define WHITE       Color(1.0f, 1.0f, 1.0f, 1.0f)
#define BOX_WIDTH   10000.0f
#if 1
    #define TOP_IMAGE    "C:/Development/projects/terrainosaurus/data/textures/faesky04u.jpg"
    #define BOTTOM_IMAGE "C:/Development/projects/terrainosaurus/data/textures/faesky04d.jpg"
    #define LEFT_IMAGE   "C:/Development/projects/terrainosaurus/data/textures/faesky04l.jpg"
    #define RIGHT_IMAGE  "C:/Development/projects/terrainosaurus/data/textures/faesky04r.jpg"
    #define FRONT_IMAGE  "C:/Development/projects/terrainosaurus/data/textures/faesky04f.jpg"
    #define BACK_IMAGE   "C:/Development/projects/terrainosaurus/data/textures/faesky04b.jpg"
#else
    #define TOP_IMAGE    "C:/Development/projects/terrainosaurus/data/textures/pos_y.bmp"
    #define BOTTOM_IMAGE "C:/Development/projects/terrainosaurus/data/textures/neg_y.bmp"
    #define LEFT_IMAGE   "C:/Development/projects/terrainosaurus/data/textures/neg_x.bmp"
    #define RIGHT_IMAGE  "C:/Development/projects/terrainosaurus/data/textures/pos_x.bmp"
    #define FRONT_IMAGE  "C:/Development/projects/terrainosaurus/data/textures/pos_z.bmp"
    #define BACK_IMAGE   "C:/Development/projects/terrainosaurus/data/textures/neg_z.bmp"
#endif

// HACK
void loadImage(const char * const filename, ColorImage& image) {
    fipImage fiImage;
    if (fiImage.load(filename)) {
        SizeType width = fiImage.getWidth();
        SizeType height = fiImage.getHeight();
        image.setSizes(width, height);
        RGBQUAD rgb;
        for (SizeType x = 0; x < width; ++x)
            for (SizeType y = 0; y < height; ++y)
            {
                fiImage.getPixelColor(x, y, &rgb);
                image(x, y) = Color(rgb.rgbRed / 255.0f, rgb.rgbGreen / 255.0f, rgb.rgbBlue / 255.0f, 1.0f);
            }
    }
}

SkyBox::SkyBox() : _displayListValid(false), _displayList(0) {
}


// Feature toggle function
bool SkyBox::toggle(const std::string & feature) {
    return false;
}


// Rendering functor
void SkyBox::operator()(terrainosaurus::Renderer & renderer) const {
    // We'll be rendering an inward-facing, axis-aligned cube
    geometry_t halfWidth = BOX_WIDTH / 2;

    // Get a reference to things we'll be using repeatedly
    Renderer::Rasterizer & rasterizer = renderer.rasterizer();
    Renderer::TexturingUnit & texUnit0 = renderer.texturingUnit(0);

    // HACK to get texturing working
    static terrainosaurus::Renderer::Texture topTexture, bottomTexture,
                                             frontTexture, backTexture,
                                             leftTexture, rightTexture;
    if (! _displayListValid) {
        ColorImage img;
        loadImage(TOP_IMAGE, img);    topTexture = renderer.createTexture(img);
        loadImage(BOTTOM_IMAGE, img); bottomTexture = renderer.createTexture(img);
        loadImage(LEFT_IMAGE, img);   leftTexture = renderer.createTexture(img);
        loadImage(RIGHT_IMAGE, img);  rightTexture = renderer.createTexture(img);
        loadImage(FRONT_IMAGE, img);  frontTexture = renderer.createTexture(img);
        loadImage(BACK_IMAGE, img);   backTexture = renderer.createTexture(img);

        _displayList = GL::glGenLists(1);

        INCA_DEBUG("Regenerating SkyBox display list")
        GL::glNewList(_displayList, GL_COMPILE);
            rasterizer.setLightingEnabled(false);
            rasterizer.setShadingModel(FlatShade);
            rasterizer.setCurrentColor(WHITE);
            GL::glEnable(GL_COLOR_MATERIAL);
            rasterizer.setFaceCullingEnabled(true);
            rasterizer.setCulledFace(Back);
            rasterizer.setTextureType(Texture2D);

        //    texUnit0.setEnabled(true);

            // Top face of the cube
            texUnit0.bind(topTexture);
            rasterizer.beginPrimitive(Quads);
                rasterizer.setCurrentNormal(Vector3D( 0.0f,  0.0f, -1.0f));
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 0.0f));
                rasterizer.vertexAt( halfWidth, -halfWidth,  halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 0.0f));
                rasterizer.vertexAt(-halfWidth, -halfWidth,  halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 1.0f));
                rasterizer.vertexAt(-halfWidth,  halfWidth,  halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 1.0f));
                rasterizer.vertexAt( halfWidth,  halfWidth,  halfWidth);
            rasterizer.endPrimitive();

            // Bottom face of the cube
            texUnit0.bind(bottomTexture);
            rasterizer.beginPrimitive(Quads);
                rasterizer.setCurrentNormal(Vector3D( 0.0f,  0.0f,  1.0f));
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 0.0f));
                rasterizer.vertexAt(-halfWidth, -halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 0.0f));
                rasterizer.vertexAt( halfWidth, -halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 1.0f));
                rasterizer.vertexAt( halfWidth,  halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 1.0f));
                rasterizer.vertexAt(-halfWidth,  halfWidth, -halfWidth);
            rasterizer.endPrimitive();

            // Left face of the cube
            texUnit0.bind(leftTexture);
            rasterizer.beginPrimitive(Quads);
                rasterizer.setCurrentNormal(Vector3D( 1.0f,  0.0f,  0.0f));
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 0.0f));
                rasterizer.vertexAt(-halfWidth, -halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 0.0f));
                rasterizer.vertexAt(-halfWidth,  halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 1.0f));
                rasterizer.vertexAt(-halfWidth,  halfWidth,  halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 1.0f));
                rasterizer.vertexAt(-halfWidth, -halfWidth,  halfWidth);
            rasterizer.endPrimitive();

            // Right face of the cube
            texUnit0.bind(rightTexture);
            rasterizer.beginPrimitive(Quads);
                rasterizer.setCurrentNormal(Vector3D(-1.0f,  0.0f,  0.0f));
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 0.0f));
                rasterizer.vertexAt( halfWidth,  halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 0.0f));
                rasterizer.vertexAt( halfWidth, -halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 1.0f));
                rasterizer.vertexAt( halfWidth, -halfWidth,  halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 1.0f));
                rasterizer.vertexAt( halfWidth,  halfWidth,  halfWidth);
            rasterizer.endPrimitive();

            // Back face of the cube
            texUnit0.bind(backTexture);
            rasterizer.beginPrimitive(Quads);
                rasterizer.setCurrentNormal(Vector3D( 0.0f,  1.0f,  0.0f));
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 0.0f));
                rasterizer.vertexAt( halfWidth, -halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 0.0f));
                rasterizer.vertexAt(-halfWidth, -halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 1.0f));
                rasterizer.vertexAt(-halfWidth, -halfWidth,  halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 1.0f));
                rasterizer.vertexAt( halfWidth, -halfWidth,  halfWidth);
            rasterizer.endPrimitive();

            // Front face of the cube
            texUnit0.bind(frontTexture);
            rasterizer.beginPrimitive(Quads);
                rasterizer.setCurrentNormal(Vector3D( 0.0f, -1.0f,  0.0f));
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 0.0f));
                rasterizer.vertexAt(-halfWidth,  halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 0.0f));
                rasterizer.vertexAt( halfWidth,  halfWidth, -halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(1.0f, 1.0f));
                rasterizer.vertexAt( halfWidth,  halfWidth,  halfWidth);
                rasterizer.setCurrentTexCoord(Point2D(0.0f, 1.0f));
                rasterizer.vertexAt(-halfWidth,  halfWidth,  halfWidth);
            rasterizer.endPrimitive();

            rasterizer.setLightingEnabled(true);
            rasterizer.setFaceCullingEnabled(false);
            rasterizer.setShadingModel(SmoothShade);
            rasterizer.setTextureType(NoTexture);

        GL::glEndList();
        _displayListValid = true;
    }

    GL::glCallList(_displayList);
}
