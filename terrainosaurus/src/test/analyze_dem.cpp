#include <terrainosaurus/terrainosaurus-common.h>

// Import application framework
#include <inca/ui.hpp>

// Import data class definitions
#include <terrainosaurus/data/TerrainSample.hpp>
using namespace terrainosaurus;

// Import raster operator definitions
#include <inca/raster/operators/select>
using namespace inca::raster;

// This is a console-only application
//#undef GUI_TOOLKIT
//#define GUI_TOOLKIT Console


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

namespace terrainosaurus {
    void drawCurves(Heightfield & hf, const CurveTracker & ct) {
        fill(hf, 0.0f);
        scalar_t maxStrength = std::log(1.0f + ct.strengthStats.max());
        for (int i = 0; i < ct.curves.size(); ++i) {
            const CurveTracker::Curve & c = ct.curves[i];
            for (int j = 0; j < c.points.size(); ++j) {
                const CurveTracker::Curve::Point & p = c.points[j];
//                hf(int(p[0]), int(p[1])) = 1.0f;
                hf(int(p[0]), int(p[1])) = std::log(1.0f + p[3]) / maxStrength;
            }
        }
    }
}


// The application class
class terrainosaurus::AnalyzeDEMApplication : public APPLICATION(GUI_TOOLKIT) {
public:
    void setup(int &argc, char **argv) {
        // Defaults
        targetLOD = TerrainLOD_Underflow;
        filename  = "";

        // Parse command-line arguments
        for (int i = 0; i < argc; ++i) {
            string arg(argv[i]);
            if (arg == "-l") {      // It's a LOD specification
                string arg2(argv[i+1]);
                if (arg2 == "10")       targetLOD = LOD_10m;
                else if (arg2 == "30")  targetLOD = LOD_30m;
                else if (arg2 == "90")  targetLOD = LOD_90m;
                else if (arg2 == "270") targetLOD = LOD_270m;
                else if (arg2 == "810") targetLOD = LOD_810m;
                else                    exit(1, "Unrecognized LOD " + arg2);

            } else {                // It's a DEM filename
                filename = arg;
            }
        }

        // Make sure we have a DEM file, and load it
        if (filename == "")
            exit(1, "You must provide at least one terrain file");
        sample = TerrainSamplePtr(new TerrainSample(filename));
        sample->ensureFileLoaded();

        // If no LOD was specified, default to the LOD in the file
        if (targetLOD == TerrainLOD_Underflow)
            targetLOD = sample->nearestLoadedLODBelow(TerrainLOD_Overflow);
        sample->ensureAnalyzed(targetLOD);

        // Write out the field descriptions
        header(std::cout);

        // Analyze the full heightfield
        dump(std::cout, (*sample)[targetLOD]);
//        histogram(std::cout, (*sample)[lod]);

        // Analyze progressively smaller chunks
        dumpSubdivisions(targetLOD, 0.5f);
        dumpSubdivisions(targetLOD, 0.25f);

        ::exit(0);
    }

    void header(std::ostream & os) {
        int fields = 60, norms = 3;
        char * field[] = {
                "EdgeCurves", "RidgeCurves",
                "SumElevation", "MinElevation", "MaxElevation", "RangeElevation", "MeanElevation", "VarElevation",
                "SumSlope", "MinSlope", "MaxSlope", "RangeSlope", "MeanSlope", "VarSlope",
                "SumEdgeLength", "MinEdgeLength", "MaxEdgeLength", "RangeEdgeLength", "MeanEdgeLength", "VarEdgeLength",
                "SumEdgeScale", "MinEdgeScale", "MaxEdgeScale", "RangeEdgeScale", "MeanEdgeScale", "VarEdgeScale",
                "SumEdgeStrength", "MinEdgeStrength", "MaxEdgeStrength", "RangeEdgeStrength", "MeanEdgeStrength", "VarEdgeStrength",
                "SumRidgeLength", "MinRidgeLength", "MaxRidgeLength", "RangeRidgeLength", "MeanRidgeLength", "VarRidgeLength",
                "SumRidgeScale", "MinRidgeScale", "MaxRidgeScale", "RangeRidgeScale", "MeanRidgeScale", "VarRidgeScale",
                "SumRidgeStrength", "MinRidgeStrength", "MaxRidgeStrength", "RangeRidgeStrength", "MeanRidgeStrength", "VarRidgeStrength",
                "FFT0", "FFT1", "FFT2", "FFT3", "FFT4", "FFT5", "FFT6", "FFT7", "FFT8", "FFT9" };
        char * norm[] = { "plain", "per_length", "per_area" };
        for (int i = 0; i < norms; ++i)
            for (int j = 0; j < fields; ++j) {
                os << norm[i] << '_' << field[j];
                if (j < fields - 1 || i < norms - 1)
                    os << ',';
            }
        os << std::endl;
    }

    void dump(std::ostream & os, const TerrainSample::LOD tsl) {
        int norms = 3;
        int norm[3];
        norm[0] = 1;
        norm[1] = tsl.sizes()[0] + tsl.sizes()[1];
        norm[2] = tsl.sizes()[0] * tsl.sizes()[1];

        for (int i = 0; i < norms; ++i) {
            os << tsl.edges().curves.size() / norm[i] << ',' << tsl.ridges().curves.size() / norm[i] << ','
               << (tsl.globalElevationStatistics() / norm[i]).stringifyElements(",") << ','
               << (tsl.globalSlopeStatistics() / norm[i]).stringifyElements(",") << ','
               << (tsl.edges().lengthStats / norm[i]).stringifyElements(",") << ','
               << (tsl.edges().scaleStats / norm[i]).stringifyElements(",") << ','
               << (tsl.edges().strengthStats / norm[i]).stringifyElements(",") << ','
               << (tsl.ridges().lengthStats / norm[i]).stringifyElements(",") << ','
               << (tsl.ridges().scaleStats / norm[i]).stringifyElements(",") << ','
               << (tsl.ridges().strengthStats / norm[i]).stringifyElements(",") << ','
               << (tsl.frequencyContent() / norm[i]).stringifyElements(",");
            if (i < norms - 1)
                os << ',';
        }
        os << std::endl;
    }

    void dumpSubdivisions(TerrainLOD lod, scalar_t fraction) {
        int cells = int(1 / fraction);
        SizeArray size = (*sample)[lod].sizes();
        size[0] = int(size[0] * fraction);
        size[1] = int(size[1] * fraction);
        if (size[0] % 2 != 0)   size[0]--;
        if (size[1] % 2 != 0)   size[1]--;

        std::cerr << "Dumping " << (cells * cells) << " subsets of " << lod << endl;

        TerrainSamplePtr chunk;
        for (int x = 0; x < cells; x++)
            for (int y = 0; y < cells; y++) {
                SizeArray bases(x * size[0], y * size[1]);
                chunk.reset(new TerrainSample(selectBS((*sample)[lod].elevations(), bases, size), lod));
                chunk->ensureAnalyzed(lod);
                dump(std::cout, (*chunk)[lod]);
            }
    }

    void histogram(std::ostream & os, const TerrainSample::LOD & tsl) {
        const TerrainSample::LOD::ScalarStatistics & stat = tsl.globalElevationStatistics();
        for (int i = 0; i < stat.histogram().size(); ++i) {
            os << stat.histogram()[i];
            if (i != stat.histogram().size() - 1)
                os << ',';
        }
        os << std::endl;
    }

protected:
    TerrainSampleConstPtr   sample;
    TerrainLOD              targetLOD;
    std::string             filename;
};


// This macro expands to a main() function that instantiates the application
// and launches it
APPLICATION_MAIN(terrainosaurus::AnalyzeDEMApplication);
