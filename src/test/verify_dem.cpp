#include <terrainosaurus/io/DEMInterpreter.hpp>
using namespace terrainosaurus;

#include <inca/raster/operators/statistic>
#include <inca/raster/operators/select>
using namespace inca::raster;

#include <iostream>
#include <string>
using namespace std;

#define TRIM 30

int main(int argc, char **argv) {
    // Gripe if we didn't get a .dem file
    if (argc < 2) {
        cerr << "Usage " << argv[0] << " <dem file>\n";
        return 1;
    }

    // Try to parse the first arg
    Heightfield hf;
    DEMInterpreter dem(hf);
    dem.filename = std::string(argv[1]);
    dem.parse();

    // See if it has data
    inca::Array<int, 2> base(TRIM),
                        extent(hf.size(0) - TRIM,
                               hf.size(1) - TRIM);
    inca::Array<scalar_t, 2> r = range(selectBE(hf, base, extent));
    if (r[0] - r[1] == scalar_t(0)) {
        cerr << argv[1] << " is corrupt\n";
        return 1;
    } else {
        cerr << argv[1] << " range(" << r[0] << " to " << r[1] << ")\n";
        return 0;
    }
}
