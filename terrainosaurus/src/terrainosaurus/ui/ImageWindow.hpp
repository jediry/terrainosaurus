/** -*- C++ -*-
 *
 * \file    ImageWindow.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The ImageWindow class is a top-level Window subclass for displaying
 *      an image.
 */

#pragma once
#ifndef TERRAINOAURUS_UI_IMAGE_WINDOW
#define TERRAINOAURUS_UI_IMAGE_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class ImageWindow;

    // Pointer typedefs
    typedef shared_ptr<ImageWindow>       ImageWindowPtr;
    typedef shared_ptr<ImageWindow const> ImageWindowConstPtr;
}

// Import superclass definition
#include <inca/ui/Window.hpp>


// Simple window displaying an image
class terrainosaurus::ImageWindow : public inca::ui::Window {
public:
    // Constructor taking an image
    template <class ImageType>
    ImageWindow(const ImageType & img,
                const std::string &title = "Image")
            : inca::ui::Window(title), _constructed(false) {

        // Setup the image
        loadImage(img);
    }
    
    // 2nd-phase construction
    void construct();

    // Setter for a real-valued, 2D image
    void loadImage(const GrayscaleImage & img);

    // Setter for a color-valued, 2D image
    void loadImage(const ColorImage & img);

    // Setter for a real-valued, 3D image
    void loadImage(const ScaleSpaceImage & img);

protected:
    bool _constructed;
    GrayscaleImage  _2dImage;
    ColorImage      _colorImage;
    ScaleSpaceImage _3dImage;
};

#endif
