/*
 * File: MapExplorer.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Description:
 */

// Initial GUI parameters
#define WINDOW_TITLE  "Terrainosaurus Map Explorer"
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480
#define FULL_SCREEN   false


// Import class definition
#include "MapExplorer.hpp"

#if 0
// Import terrain-specialized renderer
#include "TerrainosaurusRenderer.hpp"

// Import widget definitions
#include "ui/MapExplorerWidget.hpp"
#include "ui/CameraNavigationWidget.hpp"
#include "ui/CameraFlyWidget.hpp"
#include "ui/MapEditWidget.hpp"
#include "ui/AddFaceWidget.hpp"
#include "ui/AddVertexWidget.hpp"
#include "ui/ModifyPropertyWidget.hpp"
#include "ui/TranslateWidget.hpp"
#include "ui/RotateWidget.hpp"
#include "ui/ScaleWidget.hpp"

#endif

#include <fstream>
using namespace std;

// Import IOstream operators for data objects
#include "io/terrainosaurus-iostream.hpp"
#include <fstream>

// Import file-I/O exception definitions
#include <inca/io/FileExceptions.hpp>

#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/selection>
#include <inca/raster/operators/gradient>
#include <inca/raster/operators/gaussian>
#include <inca/raster/operators/transformation>
#include <inca/raster/operators/DFT>
#include <inca/raster/operators/complex>
#include <inca/raster/operators/blur>

using namespace terrainosaurus;
using namespace inca::raster;
using namespace inca::world;
using namespace inca::ui;
using namespace inca::io;

#include <complex>

typedef MultiArrayRaster< std::complex<float>, 2> ComplexImage;
typedef MultiArrayRaster< inca::math::Vector<float, 2>, 2> GradientImage;


class ImageWindow;
typedef shared_ptr<ImageWindow> ImageWindowPtr;
ImageWindowPtr globalSourceWindow, globalSourceWindow2, globalResultWindow, globalDifferenceWindow;
GrayscaleImage originalImage, originalImage2;

#define GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>
#include <inca/ui.hpp>

// XXX Hacked in ROAM impl.
#define ROAM_ME 0
#if ROAM_ME
    #include "rendering/roam/Landscape.h"
    #include "rendering/roam/Utility.h"

    class ThreeDeeWindow;
    typedef shared_ptr<ThreeDeeWindow> ThreeDeeWindowPtr;
    ThreeDeeWindowPtr roamWindow;


// Simple window calling a simplistic ROAM implementation
class ThreeDeeWindow : public inca::ui::GLUTWindow, public TerrainosaurusComponent {
public:
    // Constructor taking an image
    ThreeDeeWindow(const GrayscaleImage &hf,
                   const std::string &title = "Image Window")
            : GLUTWindow(title), heightfield(512, 512) {

        // Setup the heightfield
        gHeightMap = heightfield.array().elements();
        loadHeightfield(hf);

        SetupRC();
        SetDrawModeContext();

        // Set the size of the window
        Window::setSize(200, 200);

        // Reinitialize ROAM
        if (roamInit(gHeightMap) == 0)
            gAnimating = 1;
        else
            cerr << "Uh oh...ROAM init failed\n";
    }

    void loadHeightfield(const GrayscaleImage & hf) {
        // Start from zero
        fill(heightfield, 0);

        // Copy over the data, converting to bytes (scale and offset)
        Array<float, 2> r = range(hf);
        select(heightfield, hf.sizes()) = select(linearMap(hf, r, 5.0f, -r[0]), heightfield.sizes());
        cerr << "\nRange of heightfield is " << range(heightfield) << endl;
    }

    void reshape(int width, int height) {
        GLUTWindow::reshape(width, height);
        ChangeSize(width, height);
    }

    void display() {
//        GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        GL::glLoadIdentity();
//        GL::gluLookAt(100, 100, 100,
//                      0,   0,   0,
//                      0,   1,   0);
        RenderScene();

        GL::glutSwapBuffers();
    }
    void idle() {
        IdleFunction();
        requestRedisplay();
    }

protected:
    MultiArrayRaster<unsigned char, 2> heightfield;
    int size;
};
#endif

#include "rendering/MapRasterization.hpp"
#include "genetics/TerrainChromosome.hpp"
#include "genetics/terrain-operations.hpp"
using namespace terrainosaurus;

// Simple window displaying an image
class ImageWindow : public inca::ui::GLUTWindow, public TerrainosaurusComponent {
public:
    // Constructor taking an image
    ImageWindow(const GrayscaleImage &img,
                const std::string &title = "Image Window")
            : GLUTWindow(title), image(inca::FortranStorageOrder()) {

        // Setup the image
        loadImage(img);

        // Set the size of the window
        Window::setSize(image.size(0), image.size(1));
    }

    void loadImage(const GrayscaleImage & img) {
        cerr << "\nloadImage(): " << img.sizes() << endl;
        image = linearMap(img, Array<float, 2>(0.0f, 1.0f));
        inca::Array<float, 2> minMax = range(image);
        cerr << "Range of output is " << minMax[0] << " -> " << minMax[1] << endl;
        Window::setSize(image.size(0), image.size(1));
    }

    void reshape(int width, int height) {
        GLUTWindow::reshape(width, height);
        GL::glMatrixMode(GL_PROJECTION);
        GL::glLoadIdentity();
        GL::glMultMatrix(inca::math::screenspaceLLMatrix<double, false, false>(getSize()).elements());
        GL::glMatrixMode(GL_MODELVIEW);
    }

    void key(unsigned char k, int x, int y) {
        switch (k) {
            case ' ':
                runGA();
                break;
        }
    }

    void display() {
        GL::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GL::glRasterPos2d(0.0, 0.0);
        GL::glDrawPixels(image.size(0), image.size(1),
                     GL_LUMINANCE, GL_FLOAT, image.elements());
//        cerr << image.size(W) << 'x' << image.size(H) << endl;
//        GL::glDrawPixels(image.size(W), image.size(H),
//                         GL_RGB, GL_FLOAT, image.elements());

        GL::glutSwapBuffers();
    }

    void runGA() {
        Heightfield hf, diff;
        cerr << endl << "Running terrain GA" << endl;
        scalar_t resolution = terrainLibrary()->resolution(0);
        Point2D end(originalImage.size());
        end /= resolution;
        Block bounds(Point2D(0, 0), end);
        generateTerrain(hf, static_pointer_cast<Map const>(map()),
                        bounds, resolution);
        diff = originalImage - hf;
        globalDifferenceWindow->loadImage(diff);
        globalResultWindow->loadImage(hf);
        cerr << "RMS difference is " << rms(diff) << endl;
    }

protected:
    GrayscaleImage image;
};


// End of nasty stuff (for a while).
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


/*---------------------------------------------------------------------------*
 | Access to the singleton Application instance
 *---------------------------------------------------------------------------*/
MapExplorer & MapExplorer::instance() {
    return dynamic_cast<MapExplorer &>(Application::instance());
}


/*---------------------------------------------------------------------------*
 | Application setup functions
 *---------------------------------------------------------------------------*/
// Get command-line arguments and set up the terrainscape
void MapExplorer::setup(int &argc, char **argv) {
    // Create the selection objects
    _persistentSelection.reset(new MeshSelection());
    _transientSelection.reset(new MeshSelection());
    _persistentSelection->setElementType(MeshSelection::Edges);
    _transientSelection->setElementType(MeshSelection::Edges);

    // See if the user gave us any useful filenames
    string arg, ext;
    while (argc > 1) {
        arg = shift(argc, argv);
        ext = arg.substr(arg.length() - 4);
        if (ext == ".ttl")                  // Load the terrain from here
            loadTerrainLibrary(arg);
        else if (ext == ".map")             // Load the map from here
            loadMap(arg);
        else
            exit(1, "Unrecognized argument \"" + arg + "\"");
    }

    // If something wasn't specifed on the command line, choose a default
    if (! _map)                 createMap();
    if (! _terrainLibrary)      createTerrainLibrary();

    // Create the alignment grid
    _grid.reset(new PlanarGrid());
        grid()->minorTickSpacing = 0.5;
}



// XXX Temporary function until the regular rendering architecture is
// completed and we can use the editor again.
void MapExplorer::constructInterface() {
#define ANALYZE 0
#if ANALYZE
    const TerrainLibrary & lib = *terrainLibrary();

    ofstream os;
    os.exceptions(ofstream::failbit | ofstream::badbit);
    os.open("analysis.log");

    // Run analysis on each terrain type
    for (IndexType tti = 1; tti < lib.size(); ++tti) {
        const TerrainType & tt = *lib.terrainType(tti);
        cerr << "[" << tti << "] " << tt.name() << ": " << tt.samples.size() << " samples" << endl;
        
        for (IndexType si = 0; si < tt.samples.size(); ++si) {
            const TerrainSample & ts = *tt.samples[si];
            const Heightfield & hf = ts.heightfield(0);
            cerr << "\t[" << si << "] " << endl;

            Array<float, 2> heightRange;
            Array<float, 5> heightMeans, heightMoments;

            // Analyze the entire heightfield
            heightRange   = range(hf);
            heightMeans   = means<Heightfield, 4>(hf);
            heightMoments = moments<Heightfield, 4>(hf, heightMeans[1]);
            
            os << tt.name() << "-" << si
                // Range
               << "\t" << heightRange[0]
               << "\t" << heightRange[1]
               << "\t" << heightRange[1] - heightRange[0]

                // Means
               << "\t" << heightMeans[1]
               << "\t" << heightMeans[2]
               << "\t" << heightMeans[3]
               << "\t" << heightMeans[4]

                // Moments about the mean
               << "\t" << heightMoments[1]
               << "\t" << heightMoments[2]
               << "\t" << heightMoments[3]
               << "\t" << heightMoments[4]

                // Skewness
               << "\t" << heightMoments[3] / pow(heightMoments[2], 1.5f)

                // Kurtosis
               << "\t" << heightMoments[4] / (heightMoments[2] * heightMoments[2])
               
               << endl;

            // Analyze square subregions
            for (int bk_sz = 16; bk_sz <= 256; bk_sz <<= 1) {
                Array<int, 2> st, ed;
                int k = 0;
                for (st[0] = 0; st[0] + bk_sz < hf.size(0); st[0] += bk_sz)
                    for (st[1] = 0; st[1] + bk_sz < hf.size(1); st[1] += bk_sz) {
                        ed[0] = st[0] + bk_sz;
                        ed[1] = st[1] + bk_sz;
                        Heightfield sub = select(hf, st, ed);
                       
                        // Analyze this subregion
                        heightRange   = range(sub);
                        heightMeans   = means<Heightfield, 4>(sub);
                        heightMoments = moments<Heightfield, 4>(sub, heightMeans[1]);

                        os << tt.name() << "-" << si << "(" << bk_sz << "-" << k << ")"
                            // Range
                        << "\t" << heightRange[0]
                        << "\t" << heightRange[1]
                        << "\t" << heightRange[1] - heightRange[0]

                            // Means
                        << "\t" << heightMeans[1]
                        << "\t" << heightMeans[2]
                        << "\t" << heightMeans[3]
                        << "\t" << heightMeans[4]

                            // Moments about the mean
                        << "\t" << heightMoments[1]
                        << "\t" << heightMoments[2]
                        << "\t" << heightMoments[3]
                        << "\t" << heightMoments[4]

                            // Skewness
                        << "\t" << heightMoments[3] / pow(heightMoments[2], 1.5f)

                            // Kurtosis
                        << "\t" << heightMoments[4] / (heightMoments[2] * heightMoments[2])
                           
                        << endl;
                        
                        k++;
                    }
            }

        }
    }

    os.close();
    exit(0, "All done w/ analyzing!");

#elif 1
    typedef Array<int, 2> IndexArray;
//    originalImage = select(_terrainLibrary->terrainType(1)->samples[0]->heightfield(0), Array<int,2>(256,256));
    originalImage = _terrainLibrary->terrainType(1)->samples[0]->elevation(0);
#else
    originalImage.resize(600,500);
    for (int i = 0; i < originalImage.size(0); ++i)
        for (int j = 0; j < originalImage.size(1); ++j) {
            originalImage(i, j) = j / (originalImage.size(1) - 1.0f);
            if (std::abs(i - originalImage.size(0) / 2) + std::abs(j - originalImage.size(1) / 2) < 200)
                originalImage(i, j) = 1.0f;
//            else
//                originalImage(i, j) = 0.0f;
        }
#endif

    // Always display the heightfield as an image
    globalSourceWindow.reset(new ImageWindow(originalImage, "Original"));
    registerWindow(globalSourceWindow);

#if ROAM_ME
    roamWindow.reset(new ThreeDeeWindow(originalImage, "Heightfield"));
    registerWindow(roamWindow);
#endif

// If this is 0, do normal GA stuff. Non-zero values are for Ryan's testing.
#define WINX 0

#if WINX == 0
    globalResultWindow.reset(new ImageWindow(GrayscaleImage(100, 100), "Derived"));
    registerWindow(globalResultWindow);
    globalDifferenceWindow.reset(new ImageWindow(GrayscaleImage(100, 100), "Difference"));
    registerWindow(globalDifferenceWindow);

#else
//    originalImage2 = _terrainLibrary->terrainType(2)->samples[0]->heightfield(0);
//    globalSourceWindow2.reset(new ImageWindow(originalImage2, "Original"));
//    registerWindow(globalSourceWindow2);

    ComplexImage dft1, dft2, dft3;
    GrayscaleImage mag1, ph1, mag2, ph2, mag3, ph3;
    WindowPtr win;
    bool DCInCenter = true;

#if WINX & 1
    dft1 = DFT(originalImage);
    mag1 = blur(mag(dft1));
//    ph1  = arg(dft1);

    dft2 = DFT(originalImage2);
    mag2 = blur(mag(dft2));
//    ph2  = arg(dft2);

    //XXX
    mag3 = abs(mag2 - mag1);
    ph1 = log(mag1);
    ph2 = log(mag2);
    ph3 = log(mag3);

    cerr << endl << "Mag 1 full" << endl;
    win.reset(new ImageWindow(ph1, "Mag 1 Full"));
    win->setPosition(10, 10);
    registerWindow(win);

//    win.reset(new ImageWindow(ph1, "Ph 1 full"));
//    win->setPosition(50, 50);
//    registerWindow(win);

    cerr << endl << "Mag 2 full" << endl;
    win.reset(new ImageWindow(ph2, "Mag 2 full"));
    win->setPosition(100, 100);
    registerWindow(win);

//    win.reset(new ImageWindow(ph2, "Ph 2 full"));
//    win->setPosition(150, 150);
//    registerWindow(win);

    cerr << endl << "Mag Diff full" << endl;
    win.reset(new ImageWindow(mag3, "Mag diff full"));
    win->setPosition(200, 200);
    registerWindow(win);
    cerr << endl << "RMS diff of mag is " << rms(mag3) << endl;

#endif
#if WINX & 2
    dft2 = DFT(select(originalImage, Array<int, 2>(0), Array<int, 2>(150, 150)), DCInCenter);
    mag2 = log(mag(dft2));
    ph2  = arg(dft2);

    win.reset(new ImageWindow(mag2, "Mag half"));
    win->setPosition(20, 20);
    registerWindow(win);

    win.reset(new ImageWindow(ph2, "Ph half"));
    win->setPosition(50, 50);
    registerWindow(win);
#endif
#if WINX & 4
    dft3 = DFT(select(originalImage, Array<int, 2>(0), Array<int, 2>(75, 75)), DCInCenter);
    mag3 = log(mag(dft3));
    ph3  = arg(dft3);

    win.reset(new ImageWindow(mag3, "Mag quarter"));
    win->setPosition(30, 30);
    registerWindow(win);

    win.reset(new ImageWindow(ph3, "Ph quarter"));
    win->setPosition(60, 60);
    registerWindow(win);
#endif
#if WINX & 8
    Pixel center(originalImage.size(0) / 2, originalImage.size(1) / 2);
    Dimension diagonal = center - Pixel(0, 0);
    Pixel b1 = center,
          b2(0, center[1]),
          b3(0, 0),
          b4(center[0], 0);
    ComplexImage q1, q2, q3, q4;
    q1 = DFT(select(originalImage, b1, b1 + diagonal), DCInCenter);
    q2 = DFT(select(originalImage, b2, b2 + diagonal), DCInCenter);
    q3 = DFT(select(originalImage, b3, b3 + diagonal), DCInCenter);
    q4 = DFT(select(originalImage, b4, b4 + diagonal), DCInCenter);
    
    Pixel base(100, 100);
    Dimension offsetX(center[0] + 10, 0), offsetY(0, center[1] + 25);

    win.reset(new ImageWindow(log(mag(q1)), "Quadrant 1 M"));
    win->setPosition(base + offsetX);
    registerWindow(win);
    win.reset(new ImageWindow(log(mag(q2)), "Quadrant 2 M"));
    win->setPosition(base);
    registerWindow(win);
    win.reset(new ImageWindow(log(mag(q3)), "Quadrant 3 M"));
    win->setPosition(base + offsetY);
    registerWindow(win);
    win.reset(new ImageWindow(log(mag(q4)), "Quadrant 4 M"));
    win->setPosition(base + offsetX + offsetY);
    registerWindow(win);

    base += 2 * offsetX;
    win.reset(new ImageWindow(arg(q1), "Quadrant 1 P"));
    win->setPosition(base + offsetX);
    registerWindow(win);
    win.reset(new ImageWindow(arg(q2), "Quadrant 2 P"));
    win->setPosition(base);
    registerWindow(win);
    win.reset(new ImageWindow(arg(q3), "Quadrant 3 P"));
    win->setPosition(base + offsetY);
    registerWindow(win);
    win.reset(new ImageWindow(arg(q4), "Quadrant 4 P"));
    win->setPosition(base + offsetX + offsetY);
    registerWindow(win);

    base += 2 * offsetX;
    win.reset(new ImageWindow(log(mag(q2 - q1)), "Diff 1-2 M"));
    win->setPosition(base + offsetX);
    registerWindow(win);
    win.reset(new ImageWindow(log(mag(q3 - q2)), "Diff 2-3 M"));
    win->setPosition(base);
    registerWindow(win);
    win.reset(new ImageWindow(log(mag(q4 - q3)), "Diff 3-4 M"));
    win->setPosition(base + offsetY);
    registerWindow(win);
    win.reset(new ImageWindow(log(mag(q1 - q4)), "Diff 4-1 M"));
    win->setPosition(base + offsetX + offsetY);
    registerWindow(win);

    base += 2 * offsetY;
    win.reset(new ImageWindow(abs(log(mag(q2) - mag(q1))), "Diff 1-2 M"));
    win->setPosition(base + offsetX);
    registerWindow(win);
    win.reset(new ImageWindow(abs(log(mag(q3) - mag(q2))), "Diff 2-3 M"));
    win->setPosition(base);
    registerWindow(win);
    win.reset(new ImageWindow(abs(log(mag(q4) - mag(q3))), "Diff 3-4 M"));
    win->setPosition(base + offsetY);
    registerWindow(win);
    win.reset(new ImageWindow(abs(log(mag(q1) - mag(q4))), "Diff 4-1 M"));
    win->setPosition(base + offsetX + offsetY);
    registerWindow(win);
#endif
#if WINX & 16
    TerrainSamplePtr sample = terrainLibrary()->terrainType(1)->samples[0];
    win.reset(new ImageWindow(vmag(sample->gradient(0)), "Gradient Magnitude"));
    win->setPosition(300, 300);
    registerWindow(win);

    win.reset(new ImageWindow(vmag(sample->averageGradient(0)), "Average Gradient Magnitude"));
    win->setPosition(400, 400);
    registerWindow(win);

#endif
#if WINX & 32
    ComplexImage myDft = DFT(originalImage);
    GrayscaleImage myMag = log(clamp(mag(myDft), Array<float, 2>(1.0f, 10000.0f)));
    win.reset(new ImageWindow(myMag, "DFT"));
    win->setPosition(300, 300);
    registerWindow(win);

//    win.reset(new ImageWindow(clamp(iDFT(myDft), Array<float, 2>(0.0f, 100000.0f)), "Reconstituted Image"));
    win.reset(new ImageWindow(clamp(iDFT(DFT(originalImage)), Array<float,2>(2000.0f, 1.0e6)), "Reconstituted Image"));
    win->setPosition(400, 400);
    registerWindow(win);

#endif
#if WINX & 64
    GrayscaleImage g;
    g = select(gaussian(Array<float,2>(25.0f), Array<float,2>(2.0f)), Array<int,2>(50));
    win.reset(new ImageWindow(g, "G2"));
    win->setPosition(50, 50);
    registerWindow(win);

    g = select(gaussian(Array<float,2>(25.0f), Array<float,2>(4.0f)), Array<int,2>(50));
    win.reset(new ImageWindow(g, "G4"));
    win->setPosition(100, 100);
    registerWindow(win);

    g = select(gaussian(Array<float,2>(25.0f), Array<float,2>(8.0f)), Array<int,2>(50));
    win.reset(new ImageWindow(g, "G8"));
    win->setPosition(150, 150);
    registerWindow(win);

    g = select(gaussian(Array<float,2>(25.0f), Array<float,2>(16.0f)), Array<int,2>(50));
    win.reset(new ImageWindow(g, "G16"));
    win->setPosition(200, 200);
    registerWindow(win);

#endif
#endif
}


#if 0
// Put together our user interface
void MapExplorer::constructInterface() {
    ///////////////////////////////////////////////////////////////////////////
    // Build the widget stack

    // The MapExplorerWidget handles application events, draws the UI and
    // chooses between major application modes (editing, walk-thru, etc.)
    MapExplorerWidgetPtr explorer(new MapExplorerWidget("Map Explorer"));

    // The 2D map editing mode widget stack
    OrthographicCameraPtr camera2D(new OrthographicCamera());
        camera2D->viewWidth = 15.0;
        camera2D->viewHeight = 15.0;
        camera2D->nearClip = -1.0;
        camera2D->transform->position = Transform::Point(0.0, 0.0, 100.0);
    CameraNavigationWidgetPtr navigate2D(new CameraNavigationWidget("2D Navigation"));
    MultiplexorWidgetPtr toolSelect2D(new MultiplexorWidget("2D Tool Select"));
    MapEditWidgetPtr modifyProperty2D(new ModifyPropertyWidget("Modify Property"));
    MapEditWidgetPtr addFace2D   (new AddFaceWidget("Add Face"));
    MapEditWidgetPtr addVertex2D (new AddVertexWidget("Add Vertex"));
    MapEditWidgetPtr translate2D (new TranslateWidget("Translate"));
    MapEditWidgetPtr rotate2D    (new RotateWidget("Rotate"));
    MapEditWidgetPtr scale2D     (new ScaleWidget("Scale"));
        explorer->addWidget(navigate2D);
        explorer->toolSelect2D = toolSelect2D;
            navigate2D->widget = toolSelect2D;
            navigate2D->camera = camera2D;  // Control the 2D camera
            navigate2D->enableLook = false;     // Disable capabilities
            navigate2D->enableDolly = false;    // that don't make sense
            navigate2D->enableYaw = false;      // in two dimensions
            navigate2D->enablePitch = false;
            navigate2D->rollScale = 3.1415962 / 150.0;  // Tweak the controls
            navigate2D->zoomScale = 1.001;
                toolSelect2D->addWidget(modifyProperty2D);
                toolSelect2D->addWidget(addFace2D);
                toolSelect2D->addWidget(addVertex2D);
 //               toolSelect2D->addWidget(translate2D);
 //               toolSelect2D->addWidget(rotate2D);
 //               toolSelect2D->addWidget(scale2D);


    // The 3D fly-through mode widget stack
    PerspectiveCameraPtr cameraFly3D(new PerspectiveCamera());
        cameraFly3D->horizViewAngle = PI<Camera::scalar_t>() / 3.0;
        cameraFly3D->vertViewAngle = PI<Camera::scalar_t>() / 3.0;
        cameraFly3D->nearClip = 1.0;
        cameraFly3D->transform->position = Transform::Point(2.0, 2.0, 2.0);
        cameraFly3D->transform->lookAt(Transform::Point(0.0, 0.0, 0.0));
    CameraFlyWidgetPtr fly3D(new CameraFlyWidget("2D Flythru"));
    MapEditWidgetPtr edit3D(new MapEditWidget("3D Edit"));
        explorer->addWidget(fly3D);
            fly3D->widget = edit3D;
            fly3D->camera = cameraFly3D;


    // XXX Pass the renderer to everyone. This is ugly
    TerrainosaurusRendererPtr renderer(new TerrainosaurusRenderer());
    explorer->renderer = renderer;
    navigate2D->renderer = renderer;
    modifyProperty2D->renderer = renderer;
    addFace2D->renderer = renderer;
    addVertex2D->renderer = renderer;
    translate2D->renderer = renderer;
    rotate2D->renderer = renderer;
    scale2D->renderer = renderer;
    
    fly3D->renderer = renderer;
    edit3D->renderer = renderer;


    // Now, make a window to hold it all
    window = createWindow(explorer, WINDOW_TITLE);
    window->setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (FULL_SCREEN)    window->setFullScreen(true);
    else                window->centerOnScreen();
}
#endif


/*---------------------------------------------------------------------------*
 | I/O functions
 *---------------------------------------------------------------------------*/
// I/O operations for map objects
void MapExplorer::createMap() {
    // Make a new map
    setMap(MapPtr(new Map()));
}

void MapExplorer::loadMap(const string &filename) {
    try {
        cerr << "[" << filename << "]: loading map\n";

        // Try to open the file and scream if we fail
        std::ifstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to read map file [" << filename
              << "]: does it exist?";
            throw e;
        }

        // Create a new Map object and initialize it
        MapPtr newMap(new Map());
        newMap->terrainLibrary = _terrainLibrary;
        file >> *newMap;
        setMap(newMap);

        cerr << "[" << filename << "]: loading map complete\n";

    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}

void MapExplorer::storeMap(const string &filename) const {
    try {
        cerr << "[" << filename << "]: storing map\n";

        // Try to open the file and scream if we fail
        std::ofstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to write map file [" << filename
              << "]: check directory/file permissions";
            throw e;
        }

        // Write the map out to the file
        file << *map();

        cerr << "[" << filename << "]: storing map complete\n";

    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}


// I/O operations for terrain libraries
void MapExplorer::createTerrainLibrary() {
    // Create a new TerrainLibrary object
    TerrainLibraryPtr tl(new TerrainLibrary(3));

    // Create some sample terrain types
    tl->terrainType(0)->name = "Empty space";
    tl->terrainType(0)->color = Color(0.0f, 0.0f, 0.0f, 1.0f);
    tl->terrainType(1)->name = "Grassy Knoll";
    tl->terrainType(1)->color = Color(0.0f, 0.5f, 0.2f, 1.0f);
    tl->terrainType(2)->name = "Desert";
    tl->terrainType(2)->color = Color(0.5f, 0.5f, 0.0f, 1.0f);

    setTerrainLibrary(tl);  // Set it!
}

// Throws inca::io::FileAccessException if the file cannot be opened for reading
// Throws inca::io::FileFormatException if the file is syntactically or semantically invalid
void MapExplorer::loadTerrainLibrary(const string &filename) {
    try {
        cerr << "[" << filename << "]: loading terrain library\n";

        // Try to open the file and scream if we fail
        std::ifstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to read terrain library file [" << filename
              << "]: does it exist?";
            throw e;
        }
        
        // Create a new TerrainLibrary object and initialize it
        TerrainLibraryPtr newLib(new TerrainLibrary());
        file >> *newLib;
        setTerrainLibrary(newLib);

        cerr << "[" << filename << "]: loading terrain library complete\n";

    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}

// Throws inca::io::FileAccessException if the file cannot be opened for writing
void MapExplorer::storeTerrainLibrary(const string &filename) const {
    try {
        cerr << "[" << filename << "]: storing terrain library\n";

        // Try to open the file and scream if we fail
        std::ofstream file(filename.c_str());
        if (! file) {
            FileAccessException e(filename);
            e << "Unable to write terrain library file [" << filename
              << "]: check directory/file permissions";
            throw e;
        }

        // Write the library out to the file
        file << *terrainLibrary();

        cerr << "[" << filename << "]: storing terrain library complete\n";
    } catch (const StreamException &e) {
        cerr << "[" << filename << "]: " << e << endl;
    }
}


/*---------------------------------------------------------------------------*
 | Data and UI components
 *---------------------------------------------------------------------------*/
void MapExplorer::setMap(MapPtr m) {
    // Now, we point to this map, not that one
    _map = m;

    // Attach the terrain library, if there is one
    if (_terrainLibrary) _map->terrainLibrary = _terrainLibrary;

    // Now, attach this new map to each of the selections
    _persistentSelection->setMap(map());
    _transientSelection->setMap(map());
}

void MapExplorer::setTerrainLibrary(TerrainLibraryPtr tl) {
    // Now, we point to this library, not that one
    _terrainLibrary = tl;

    // Attach it to the map, if there is one
    if (_map) _map->terrainLibrary = _terrainLibrary;

    // Ensure that the current TT belongs to this library
    if (tl == NULL || tl->size() == 0)
        // Crud. There ARE no TTs
        _currentTerrainType = TerrainTypePtr();
    else {
        // Pick the first TerrainType (if possible), since the zeroth one
        // a special TT intended only for empty space
        IndexType index = (tl->size() >= 2) ? 1 : 0;
        _currentTerrainType = _terrainLibrary->terrainType(index);
    }
}

void MapExplorer::setCurrentTerrainType(TerrainTypePtr tt) {
    // This becomes our selected TT
    _currentTerrainType = tt;
}


// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::MapExplorer);
