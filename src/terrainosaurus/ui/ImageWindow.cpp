/** -*- C++ -*-
 *
 * \file    ImageWindow.cpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "ImageWindow.hpp"

// Initial configuration constants
#define WINDOW_SIZE         Dimension(400, 300)
#define FULL_SCREEN         false
#define BACKGROUND_COLOR    Color(0.3f, 0.1f, 0.1f, 1.0f)


// Declare the specialized Widget subclass that will serve as the top-level
// widget for this kind of Window
namespace terrainosaurus {
    // Forward declaration
    class ImageWindowWidget;
    typedef shared_ptr<ImageWindowWidget> ImageWindowWidgetPtr;
}

// Import UI object definitions
#include <inca/ui/widgets/PassThruWidget.hpp>

// Import image processing functions
#include <inca/raster/operators/linear_map>

// HACK Import rendering mechanism
#define INCA_GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>

using namespace inca;
using namespace inca::ui;
using namespace terrainosaurus;


class terrainosaurus::ImageWindowWidget : public inca::ui::PassThruWidget {
/*---------------------------------------------------------------------------*
 | Constructors & initialization
 *---------------------------------------------------------------------------*/
public:
    // Constructor
    explicit ImageWindowWidget(const std::string & nm = std::string())
            : PassThruWidget(nm) { }
    
    void construct() {
        renderer().addAutoClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer().rasterizer().setBackgroundColor(BACKGROUND_COLOR);
        renderer().rasterizer().setDepthBufferingEnabled(false);
    }

    // Setter for a real-valued, 2D image
    void loadImage(const GrayscaleImage & img) {
        typedef GrayscaleImage::ElementType Scalar;
        INCA_INFO("Setting plain image: (" << img.sizes() << ")")
        _2dImage = linear_map(img, inca::Array<Scalar, 2>(Scalar(0), Scalar(1)));
//        image = img;
        _imageMode = 0;

        RenderableSurfacePtr s = surface();
        WindowPtr w = static_pointer_cast<Window>(s->parent());
        w->setSize(_2dImage.size(0), _2dImage.size(1));
    }

    // Setter for a color-valued, 2D image
    void loadImage(const ColorImage & img) {
        typedef ColorImage::ElementType Scalar;
        INCA_INFO("Setting color image: (" << img.sizes() << ")")
        _colorImage = img;
        _imageMode = 1;

        RenderableSurfacePtr s = surface();
        WindowPtr w = static_pointer_cast<Window>(s->parent());
        w->setSize(_colorImage.size(0), _colorImage.size(1));
    }

    // Setter for a real-valued, 3D image
    void loadImage(const ScaleSpaceImage & img) {
        typedef ScaleSpaceImage::ElementType Scalar;
        INCA_INFO("Setting scale-space image: (" << img.sizes() << ")")
        _3dImage = linear_map(img, inca::Array<Scalar, 2>(Scalar(0), Scalar(1)));
//        scaleImage = img;
        _imageMode = 2;
        _layer = 0;

        RenderableSurfacePtr s = surface();
        WindowPtr w = static_pointer_cast<Window>(s->parent());
        w->setSize(_3dImage.size(0), _3dImage.size(1));
    }
    
    void render() const {
        GL::glRasterPos2d(0.0, 0.0);
        switch (_imageMode) {
        case 0:     // Real, 2D
            GL::glDrawPixels(_2dImage.size(0), _2dImage.size(1),
                             GL_LUMINANCE, GL_FLOAT, _2dImage.elements());
            break;
        case 1:     // Color, 2D
            GL::glDrawPixels(_colorImage.size(0), _colorImage.size(1),
                             GL_RGBA, GL_FLOAT, _colorImage.elements());
            break;
        case 2:     // Real, 3D
            GL::glDrawPixels(_3dImage.size(0), _3dImage.size(1),
                             GL_LUMINANCE, GL_FLOAT, _3dImage.elements()
                 + _3dImage.array().indexOf(inca::Array<int, 3>(0, 0, _layer)));
            break;
        }
    }

    void setSize(SizeType w, SizeType h) {
        setSize(Dimension(w, h));
    }
    void setSize(Dimension d) {
        GL::glMatrixMode(GL_PROJECTION);
        GL::glLoadIdentity();
        GL::glMultMatrix(inca::math::screenspaceLLMatrix<double, false, false>(d).elements());
        GL::glMatrixMode(GL_MODELVIEW);
    }


protected:
    int             _imageMode, _layer;
    GrayscaleImage  _2dImage;
    ColorImage      _colorImage;
    ScaleSpaceImage _3dImage;
};


void ImageWindow::construct() {
    // HACK : call superclass version...need other mechanism
    Window::construct();

    // Set up the window
    setSize(WINDOW_SIZE);
    setFullScreen(FULL_SCREEN);

    // Attach the widget chain
    ImageWindowWidgetPtr w(new ImageWindowWidget("Image Window"));
    surface()->setWidget(w);

    // Pass along anything that we got from our constructor
    if (_2dImage.size() != 0)
        w->loadImage(_2dImage);
    if (_colorImage.size() != 0)
        w->loadImage(_colorImage);
    if (_3dImage.size() != 0)
        w->loadImage(_3dImage);
        
    _constructed = true;    // This should be more general-like
}

// Setter for a real-valued, 2D image
void ImageWindow::loadImage(const GrayscaleImage & img) {
    _2dImage = img;
    if (_constructed) {
        WidgetPtr w = surface()->widget();
        ImageWindowWidgetPtr iw = static_pointer_cast<ImageWindowWidget>(w);
        iw->loadImage(img);
    }
}

// Setter for a color-valued, 2D image
void ImageWindow::loadImage(const ColorImage & img) {
    _colorImage = img;
    if (_constructed) {
        WidgetPtr w = surface()->widget();
        ImageWindowWidgetPtr iw = static_pointer_cast<ImageWindowWidget>(w);
        iw->loadImage(img);
    }
}

// Setter for a real-valued, 3D image
void ImageWindow::loadImage(const ScaleSpaceImage & img) {
    _3dImage = img;
    if (_constructed) {
        WidgetPtr w = surface()->widget();
        ImageWindowWidgetPtr iw = static_pointer_cast<ImageWindowWidget>(w);
        iw->loadImage(img);
    }
}
