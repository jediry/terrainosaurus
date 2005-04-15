#ifndef TERRAINOAURUS_UI_IMAGE_WINDOW
#define TERRAINOAURUS_UI_IMAGE_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class ImageWindow;
}


// Import metaprogramming tools
#include <inca/util/multi-dimensional-macros.hpp>
#include <inca/util/metaprogramming/macros.hpp>


// Simple window displaying an image
class terrainosaurus::ImageWindow : public WINDOW(GUI_TOOLKIT) {
public:
    // Constructor taking an image
    template <class ImageType>
    ImageWindow(const ImageType &img,
                const std::string &title = "Real Image")
            : WINDOW(GUI_TOOLKIT)(title) {

        // Setup the image
        loadImage(img);
    }

    // Setter for a real, 2D image
    template <class ImageType>
    void loadImage(const ImageType & img,
            ENABLE_FUNCTION_IF( EQUAL( INT(ImageType::dimensionality), INT(2)))) {
        GrayscaleImage tmp = img;

        typedef typename GrayscaleImage::ElementType Scalar;
        INCA_INFO("Setting plain image: (" << img.sizes() << ")")
        plainImage = linear_map(tmp, inca::Array<Scalar, 2>(Scalar(0), Scalar(1)));
//        image = img;
        Window::setSize(plainImage.size(0), plainImage.size(1));
        imageMode = 0;
    }

    // Setter for a real, 3D image
    template <class ImageType>
    void loadImage(const ImageType & img,
            ENABLE_FUNCTION_IF( EQUAL( INT(ImageType::dimensionality), INT(3)))) {
        typedef typename ImageType::ElementType Scalar;

        INCA_INFO("Setting scale-space image: (" << img.sizes() << ")")
        scaleImage = linear_map(img, inca::Array<Scalar, 2>(Scalar(0), Scalar(1)));
//        scaleImage = img;
        Window::setSize(scaleImage.size(0), scaleImage.size(1));
        imageMode = 1;
        layer = 0;
    }

    void reshape(int width, int height) {
        GLUTWindow::reshape(width, height);
        GL::glMatrixMode(GL_PROJECTION);
        GL::glLoadIdentity();
        GL::glMultMatrix(inca::math::screenspaceLLMatrix<double, false, false>(getSize()).elements());
        GL::glMatrixMode(GL_MODELVIEW);
    }

    void display() {
        GL::glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GL::glRasterPos2d(0.0, 0.0);
        switch (imageMode) {
        case 0:     // Real, 2D
            GL::glDrawPixels(plainImage.size(0), plainImage.size(1),
                             GL_LUMINANCE, GL_FLOAT, plainImage.elements());
            break;
        case 1:     // Real, 3D
            GL::glDrawPixels(scaleImage.size(0), scaleImage.size(1),
                             GL_LUMINANCE, GL_FLOAT, scaleImage.elements()
                 + scaleImage.array().indexOf(inca::Array<int, 3>(0, 0, layer)));
            break;
        }
        GL::glutSwapBuffers();
    }

    void mouseButton(int button, int state, int x, int y) {
        y = this->getSize()[1] - y;

        if (state == GLUT_DOWN) {
            switch (imageMode) {
            case 0:
                plainImage(x, y) = 1.0f;
                break;
            case 1:
                if (button == GLUT_LEFT_BUTTON && layer < scaleImage.size(2) - 1)
                    ++layer;
                if (button == GLUT_RIGHT_BUTTON && layer > 0)
                    --layer;
                INCA_INFO("Displaying scale layer " << layer)
                break;
            }
            requestRedisplay();
        }
    }


protected:
    int             imageMode, layer;
    GrayscaleImage  plainImage;
    ScaleSpaceImage scaleImage;
};


// Clean up the preprocessor's namespace
#define UNDEFINE_INCA_MULTI_DIM_MACROS
#include <inca/util/multi-dimensional-macros.hpp>
#define UNDEFINE_INCA_METAPROGRAMMING_MACROS
#include <inca/util/metaprogramming/macros.hpp>

#endif
