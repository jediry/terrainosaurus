#include <terrainosaurus/terrainosaurus-common.h>

#include <terrainosaurus/io/DEMInterpreter.hpp>

#include <terrainosaurus/data/TerrainSample.hpp>

#include <inca/raster/operators/resample>
#include <inca/raster/operators/linear_map>
#include <inca/raster/operators/magnitude>
#include <inca/raster/operators/fourier>
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/select>
#include <inca/raster/generators/constant>

// Import application framework
#include <inca/ui.hpp>

// Import OpenGL
#define GL_HPP_IMPORT_GLUT
#include <inca/integration/opengl/GL.hpp>

#include <inca/util/metaprogramming/macros.hpp>
#include <inca/util/multi-dimensional-macros.hpp>

#include <terrainosaurus/ui/ImageWindow.hpp>

using namespace inca::raster;
using namespace terrainosaurus;

typedef MultiArrayRaster<scalar_t, 3> HyperImage;


// Forward declaration
namespace terrainosaurus {
    class AnalyzeDEMApplication;
}


    // FIXME: hacked in function
    template <typename T, SizeType dim>
    inca::Array<T, dim> operator/(const inca::Array<T, dim> & a, int i) {
        inca::Array<T, dim> result;
        for (int d = 0; d < dim; ++d)
            result[d] = a[d] / i;
        return result;
    }


// The application class
class terrainosaurus::AnalyzeDEMApplication : public APPLICATION(GUI_TOOLKIT) {
public:
    void setup(int &argc, char **argv) {
        if (argc <= 1)
           header(std::cout);
//             exit(1, "You must provide at least one .dem file");
        else {

            // Write out the field descriptions
            header(std::cout);

            // Analyze the full heightfield
            filename = argv[1];
            sample = TerrainSampleConstPtr(new TerrainSample(filename));
            sample->ensureLoaded(LOD_30m);
            sample->ensureAnalyzed(LOD_30m);
            dump(std::cout, sample);

            // Analyze progressively smaller chunks
            TerrainSamplePtr chunk;
            SizeArray size;
            scalar_t fraction(0.75);
            size[0] = SizeType(sample->sizes(LOD_30m)[0] * fraction);
            size[1] = SizeType(sample->sizes(LOD_30m)[1] * fraction);
            chunk.reset(new TerrainSample(select(sample->elevations(LOD_30m), size), LOD_30m));
            chunk->ensureAnalyzed(LOD_30m);
            dump(std::cout, chunk);

            fraction = scalar_t(0.5);
            size[0] = SizeType(sample->sizes(LOD_30m)[0] * fraction);
            size[1] = SizeType(sample->sizes(LOD_30m)[1] * fraction);
            chunk.reset(new TerrainSample(select(sample->elevations(LOD_30m), size), LOD_30m));
            chunk->ensureAnalyzed(LOD_30m);
            dump(std::cout, chunk);

            fraction = scalar_t(0.25);
            size[0] = SizeType(sample->sizes(LOD_30m)[0] * fraction);
            size[1] = SizeType(sample->sizes(LOD_30m)[1] * fraction);
            chunk.reset(new TerrainSample(select(sample->elevations(LOD_30m), size), LOD_30m));
            chunk->ensureAnalyzed(LOD_30m);
            dump(std::cout, chunk);

            fraction = scalar_t(0.1);
            size[0] = SizeType(sample->sizes(LOD_30m)[0] * fraction);
            size[1] = SizeType(sample->sizes(LOD_30m)[1] * fraction);
            chunk.reset(new TerrainSample(select(sample->elevations(LOD_30m), size), LOD_30m));
            chunk->ensureAnalyzed(LOD_30m);
            dump(std::cout, chunk);
        }
//        ::exit(0);
    }


    void header(std::ostream & os) {
        char * field[] = {
                "EdgeCurves", "RidgeCurves",
                "SumElevation", "MinElevation", "MaxElevation", "MeanElevation", "VarElevation",
                "SumSlope", "MinSlope", "MaxSlope", "MeanSlope", "VarSlope",
                "SumEdgeLength", "MinEdgeLength", "MaxEdgeLength", "MeanEdgeLength", "VarEdgeLength",
                "SumEdgeScale", "MinEdgeScale", "MaxEdgeScale", "MeanEdgeScale", "VarEdgeScale",
                "SumEdgeStrength", "MinEdgeStrength", "MaxEdgeStrength", "MeanEdgeStrength", "VarEdgeStrength",
                "SumRidgeLength", "MinRidgeLength", "MaxRidgeLength", "MeanRidgeLength", "VarRidgeLength",
                "SumRidgeScale", "MinRidgeScale", "MaxRidgeScale", "MeanRidgeScale", "VarRidgeScale",
                "SumRidgeStrength", "MinRidgeStrength", "MaxRidgeStrength", "MeanRidgeStrength", "VarRidgeStrength",
                "FFT0", "FFT1", "FFT2", "FFT3", "FFT4", "FFT5", "FFT6", "FFT7", "FFT8", "FFT9" };
        char * tag[] = { "plain", "per_width", "per_height", "per_area" };
//        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 52; ++j) {
                os << field[j];
//                os << tag[i] << '_' << field[j];
//                if (j < 51 || i < 4)
                if (j < 51)
                    os << ',';
            }
        os << std::endl;
    }

    void dump(std::ostream & os, TerrainSampleConstPtr s) {
        int div[4];
        div[0] = 1;
        div[1] = s->sizes(LOD_30m)[0];
        div[2] = s->sizes(LOD_30m)[1];
        div[3] = s->sizes(LOD_30m)[0] * s->sizes(LOD_30m)[1];

        for (int i = 0; i < 1; ++i) {
            os << s->edges.curves.size() / div[i] << ',' << s->ridges.curves.size() / div[i] << ','
               << (s->globalElevationStatistics(LOD_30m) / div[i]).stringifyElements(",") << ','
               << (s->globalSlopeStatistics(LOD_30m) / div[i]).stringifyElements(",") << ','
               << (s->edges.lengthStats / div[i]).stringifyElements(",") << ','
               << (s->edges.scaleStats / div[i]).stringifyElements(",") << ','
               << (s->edges.strengthStats / div[i]).stringifyElements(",") << ','
               << (s->ridges.lengthStats / div[i]).stringifyElements(",") << ','
               << (s->ridges.scaleStats / div[i]).stringifyElements(",") << ','
               << (s->ridges.strengthStats / div[i]).stringifyElements(",") << ','
               << (s->frequencyContent / div[i]).stringifyElements(",");
//            if (i < 3)
//                os << ',';
        }
        os << std::endl;
    }


    void constructInterface() {
        typedef inca::Array<IndexType, 2> IndexArray;
//        Heightfield output = sample->elevations(0);
/*        Heightfield output(346, 346);
        fill(output, 0.0f);
        selectBE(output, IndexArray(50,50), IndexArray(100,100)) = select(constant<scalar_t, 2>(1.0f), IndexArray(100, 100));
        inca::ui::WindowPtr win1(new ImageWindow(output, "Heightfield"));
        registerWindow(win1);*/
//         inca::ui::WindowPtr winA(new ImageWindow(log(cmagnitude(dft(output))+1), "DFT"));
//         registerWindow(winA);
//         inca::ui::WindowPtr winB(new ImageWindow(idft(dft(output)), "DFT n back"));
//         registerWindow(winB);

/*        inca::ui::WindowPtr win2(new ImageWindow(sample->ridgeImage, "Ridges"));
        registerWindow(win2);
        inca::ui::WindowPtr win3(new ImageWindow(sample->edgeImage, "Edges"));
        registerWindow(win3);
        inca::ui::WindowPtr win4(new ImageWindow(sample->hyper, "Hyper"));
        registerWindow(win4);*/
/*        Heightfield hf = select(sample->elevations(LOD_30m), IndexArray(45, 62));
        inca::ui::WindowPtr w;
        w.reset(new ImageWindow(hf, "Actual Size"));
        registerWindow(w);
        w.reset(new ImageWindow(resample(hf, 2), "x2"));
        registerWindow(w);
        w.reset(new ImageWindow(resample(hf, 1.1f), "x1.1"));
        registerWindow(w);
        w.reset(new ImageWindow(resample(hf, 0.5f), "x0.5"));
        registerWindow(w);
        w.reset(new ImageWindow(resample(hf, Array<float, 2>(0.5f, 1.8f)), "diff"));
        registerWindow(w);*/
    }

protected:
    TerrainSampleConstPtr sample;
    std::string filename;
};


// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::AnalyzeDEMApplication);
