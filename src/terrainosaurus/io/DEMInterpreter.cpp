/*
 * File: DEMInterpreter.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "DEMInterpreter.hpp"
using namespace terrainosaurus;

// Import container definitions
#include <vector>

// Import standard library math functions
#include <cmath>

// Import IOstream file stream definitions
#include <fstream>

// Import file-related exception definitions
#include <inca/io/FileExceptions.hpp>
using namespace inca::io;

// How big is our buffer?
const std::size_t DEMInterpreter::BUFFER_SIZE = 41;

// How big is each record type?
const std::size_t DEMInterpreter::CHUNK_SIZE = 1024;


// Constructor
DEMInterpreter::DEMInterpreter(Heightfield & hf)
    : buffer(new char[BUFFER_SIZE]), raster(hf), filename(this) { }
    

// Parse from the file named with the filename property
void DEMInterpreter::parse() {
    std::string path(filename);
    std::ifstream file(path.c_str());
    if (! file) {
        FileAccessException e(path);
        e << "Unable to read DEM file [" << path
          << "]: check directory/file permissions";
        throw e;
    }

    parse(file);
    file.close();
}


// Parse the file, calling the appropriate callbacks
void DEMInterpreter::parse(std::istream & is) {
    // Header information
    parseRecordTypeA(is);

    // Keep count of how many CHUNK_SIZE blocks we've used
    std::size_t chunkCount = 0;

    // There may or may not be '\n's, so this can affect our byte count
    std::size_t eolCount = 0;

    // Elevation profile data in S -> N columns
    IndexType r = 1;
    for (IndexType c = 1; c <= IndexType(raster.size(0)); c++) {
//    for (IndexType c = 1; c <= IndexType(raster.width()); c++) {
        // This is where we last were
        std::size_t baseline = CHUNK_SIZE * chunkCount + eolCount;

        // Figure out how far we've advanced
        std::size_t pos = is.tellg();
        while (baseline < pos) {
            // Advance to the next chunk
            baseline += CHUNK_SIZE;
            chunkCount++;
            is.seekg(baseline);

            // See if there's a newline that we must account for
            int i = is.get();
            if (i == '\n') {
                eolCount++;     // Gotcha!
                baseline++;
            } else {
                is.unget();     // Oops! Sorry to disturb you, ma'am
            }
        }

        // Parse the record
        parseRecordTypeB(r, c, is);
    }

//    parseRecordTypeC(is);
}

// Parses an A-type (header) record
void DEMInterpreter::parseRecordTypeA(std::istream & is) {
    // Element 1: File name -- 40 characters
    //      "The authorized digital cell name followed by a comma, space, and
    //       the two-character State designator(s) separated by hyphens.
    //       Abbreviations for other countries, such as Canada and Mexico,
    //       shall not be represented in the DEM header."
    std::string fileName = readChars(40, is);
//    INCA_DEBUG("File name: " << fileName)

    // Element 2: Free-format text -- 40 characters
    //      "Free format descriptor field, contains useful information related
    //       to digital processes such as digitizing instrument, photo codes,
    //       slot widths, etc."
    std::string comment = readChars(40, is);
//    INCA_DEBUG("Comment: " << comment)

    // Element 3: Filler space -- 29 characters
    //      "Blank fill."
    std::string filler1 = readChars(29, is);
    if (filler1 != std::string(29, ' '))
        INCA_WARNING("DEM Filler(29): \"" << filler1 << '"')

    // Element 4: SE geographic corner -- 26 bytes => 2 * (I4 I2 F7.4)
    //      "SE geographic quadrangle corner ordered as:
    //          x = longitude = SDDDMMSS.SSSS
    //          y = latitude  = SDDDMMSS.SSSS
    //       (negative sign (S) right-justified, no leading zeroes,
    //        plus sign (S) implied)."
    std::string seCorner = readChars(26, is);
//    INCA_DEBUG("SE corner: " << seCorner)

    // Element 5: Process code -- 1 byte => 1 integer
    //      "1 = Autocorrelation RESAMPLE simple bilinear
    //       2 = Manual profile GRIDEM simple bilinear
    //       3 = DLG/hypsography CTOG 8-direction linear
    //       4 = Interpolation from photogrammetric system contours
    //           DCASS 4-direction linear
    //       5 = DLG/hypsography LINETRACE, LT4X complex linear
    //       6 = DLG/hypsography CPS-3, ANUDEM, GRASS complex polynomial
    //       7 = Electronic imaging (non-photogrammetric),
    //           active or passive, sensor systems"
    int processCode = readInteger(1, is);
//    INCA_DEBUG("Process code: " << processCode)

    // Element 6: Filler -- 1 byte
    //      "Blank fill."
    std::string filler2 = readChars(1, is);
    if (filler2 != " ")
        INCA_WARNING("DEM Filler(1): '" << filler2 << "'")

    // Element 7: Sectional indicator -- 3 characters
    //      "This code is specific to 30-minute DEMs. Identifies
    //       1:100,000-scale sections. (See appendix 2-I
    //       [of the DEM specification].)"
    std::string sectionalIndicator = readChars(3, is);
//    INCA_DEBUG("Sectional indicator: " << sectionalIndicator)

    // Element 8: Origin code -- 4 characters
    //      "Free format Mapping Origin Code. Example: MAC, WMC, MCMC, RMMC,
    //       FS, BLM, CONT (contractor), XX (state postal code)."
    std::string originCode = readChars(4, is);
//    INCA_DEBUG("Origin code: " << originCode)

    // Element 9: DEM level code -- 6 bytes => 1 integer
    //      "Code 1 = DEM-1
    //            2 = DEM-2
    //            3 = DEM-3
    //            4 = DEM-4"
    int demLevel = readInteger(6, is);
//    INCA_DEBUG("DEM level code: " << demLevel)

    // Element 10: Elevation pattern code -- 6 bytes => 1 integer
    //      "Code 1 = regular
    //            2 = random, reserved for future use"
    int elevationPattern = readInteger(6, is);
//    INCA_DEBUG("Elevation pattern: " << elevationPattern)

    // Element 11: Ground planimetric coord. system code -- 6 bytes => 1 integer
    //      "Code 0 = Geographic
    //            1 = UTM
    //            2 = State plane
    //            3 - 20 : see appendix G
    //       Code 0 represents the geographic (latitude/longitude) suystem for
    //       30-minute, 1-degree, and Alaska DEMs. Code 1 represents the
    //       current use of the UTM coordinate system for 7.5-minute DEMs."
    int coordinateSystem = readInteger(6, is);
//    INCA_DEBUG("Coordinate system: " << coordinateSystem)

    // Element 12: Ground planimetric zone code -- 6 bytes => 1 integer
    //      "Codes for State plane and UTM coordinate zones are given in
    //       appendices E and F for 7.5-minute DEMs. Code is set to zero if
    //       element 5 is also set to zero, defining data as geographic."
    int coordinateZone = readInteger(6, is);
//    INCA_DEBUG("Coordinate zone: " << coordinateZone)

    // Element 13: Map projection parameters -- 360 bytes => 15 24-byte reals
    //      "Definition of parameters for various projections is given in
    //       Appendix F. All 15 fields of this element are set to zero and
    //       should be ignored when goegraphic, UTM, or State plane coordinates
    //       are coded in data element [11]."
    double projectionParameters[15];
    for (IndexType i = 0; i < 15; i++) {
        projectionParameters[i] = readDouble(24, is);
//        INCA_DEBUG("Projection parameter " << i << ": " << projectionParameters[i])
    }

    // Element 14: Ground planimetric unit of measure -- 6 bytes => 1 integer
    //      "Code 0 = radians
    //            1 = feet
    //            2 = meters
    //            3 = arc-seconds
    //       Normally set to code 2 for 7.5-minute DEMs. Always set to code
    //       3 for 30-minute, 1-degree and Alaska DEMs."
    horizontalUnits = readInteger(6, is);
//    INCA_DEBUG("Horizontal units: " << horizontalUnits)

    // Element 15: Elevation unit of measure -- 6 bytes => 1 integer
    //      "Code 1 = feet
    //            2 = meters
    //       Normally code 2 (meters) for 7.5-minute, 30-minute, 1-degree
    //       and Alaska DEMs."
    verticalUnits = readInteger(6, is);
    INCA_DEBUG("Vertical units: " << (verticalUnits == 1 ? "feet" : "meters"))

    // Element 16: Number of sides in boundary polygon -- 6 bytes => 1 integer
    //      "Set to n = 4."
    int boundaryPolygonSides = readInteger(6, is);
//    INCA_DEBUG("Boundary polygon sides: " << boundaryPolygonSides)

    // Element 17: Quadrangle corner coordinates -- 192 bytes => 4 24-byte reals
    //      "The coordinates of the quadrangle corners are ordered in a
    //       clockwise direction beginning with the southwest corner. The array
    //       is stored as pairs of eastings and northings."
    double corners[4][2];
    for (IndexType i = 0; i < 4; i++) {
        corners[i][0] = readDouble(24, is);
        corners[i][1] = readDouble(24, is);
        if (i == 0) {
            extents[0][0] = corners[0][0];
            extents[1][0] = corners[0][0];
            extents[0][1] = corners[0][1];
            extents[1][1] = corners[0][1];
        } else {
            if (corners[i][0] < extents[0][0])  extents[0][0] = corners[i][0];
            if (corners[i][0] > extents[1][0])  extents[1][0] = corners[i][0];
            if (corners[i][1] < extents[0][1])  extents[0][1] = corners[i][1];
            if (corners[i][1] > extents[1][1])  extents[1][1] = corners[i][1];
        }
//        INCA_DEBUG("Corner " << i << ": " << corners[i][0] << ", " << corners[i][1])
    }
//    INCA_DEBUG("Min extents: " << extents[0][0] << ", " << extents[0][1])
//    INCA_DEBUG("Max extents: " << extents[1][0] << ", " << extents[1][1])
//    std::vector<Handler::Point2D> vertices;
//    for (IndexType i = 0; i < 4; i++)
//        vertices.push_back(Handler::Point2D(corners[i][0], corners[i][1]));
//    handler->trimmingPolygon = Handler::Polygon(vertices);

    // Element 18: Elevation range -- 48 bytes => 2 24-byte reals
    //      "Minimum and maximum elevations for the DEM. The values are in the
    //       unit of measure given by data element [15] in this record and are
    //       the algebraic result of the method outlined in data element 6,
    //       logical record B [forward reference to record type B]."
    elevationExtrema[2];
    for (IndexType i = 0; i < 2; i++)
        elevationExtrema[i] = readDouble(24, is);
    INCA_DEBUG("Elevation extrema: " << elevationExtrema[0]
               << " => " << elevationExtrema[1])


    // Element 19: Deviation angle -- 24 bytes => 1 24-byte real
    //      "Counterclockwise angle (in radians) from the primary axis of
    //       ground planimetric reference to the primary axis of the DEM local
    //       reference system. Set to zero [if aligned] with the coordinate
    //       system specified in element [11]."
    deviationAngle = readDouble(24, is);
//    INCA_DEBUG("Deviation angle: " << deviationAngle)

    // Element 20: Accuracy record -- 6 bytes => 1 boolean
    //      "Accuracy code for elevations.
    //       Code 0 = unknown accuracy
    //            1 = accuracy information is given in logical record type C."
    bool hasAccuracyRecord = readBoolean(6, is);
//    INCA_DEBUG("Has accuracy record: " << hasAccuracyRecord)

    // Element 21: Spatial resolution -- 36 bytes => 3 12-byte reals
    //      "A three-element array of DEM spatial resolution for x, y, z.
    //       Values are expressed in units of resolution. The units
    //       of...[specification document missing text here]...with
    //       those indicated by data elements [14] and [15].
    //
    //      "Only integer values are permitted for the x and y resolutions.
    //       For all USGS DEMs except the 1-degree DEM, z resolutions of
    //       1 decimal place for feet and 2 decimal places for meters are
    //       permitted. Some typical arrays are:
    //          [2, 2, 1] and [2, 2, .1] for 30-minute DEM
    //          [3, 3, 1] for 1-degree DEM
    //          [2, 1, 1] and [2, 1, .1] for 7.5-minute Alaska DEM
    //          [3, 2, 1] and [3, 2, .1] for 15-minute Alaska DEM"
    resolution[3];
    for (IndexType i = 0; i < 3; i++) {
        resolution[i] = readDouble(12, is);
//        INCA_DEBUG("Resolution " << i << ": " << resolution[i])
    }
//    handler->xResolution = resolution[0];
//    handler->yResolution = resolution[1];
//    handler->zResolution = resolution[2];

    // Element 22: Grid size -- 12 bytes => 2 integers
    //      "A two element array containing the number of rows and columns
    //       (m, n) of profiles in the DEM. When the row value m is set to
    //       1, the n value describes the number of columns in the DEM file."
    int rows = readInteger(6, is);
    int columns = readInteger(6, is);
    double colGuess = (extents[1][0] - extents[0][0]) / resolution[0];
    double rowGuess = (extents[1][1] - extents[0][1]) / resolution[1];
//    INCA_DEBUG("Profiles: " << rows << " rows x " << columns << " columns")
//    INCA_DEBUG("Guess: " << rowGuess << " rows x " << colGuess << " columns")
    raster.setSizes(inca::SizeType(columns), inca::SizeType(rowGuess + 1));


    /*************************************************************************
     * At this point, the original specification stops. The 1988 format adds
     * the following additional fields.
     *************************************************************************/


    // Element 23: Largest primary contour interval -- 5 bytes => 1 integer
    //      "Present only if two or more primary intervals exist
    //       (level 2 DEMs only)."
    int largestInterval = readInteger(5, is);
    if (std::string(buffer.get()) == "     ") {
        INCA_INFO("This DEM appears to follow the pre-1988 format")
        return;
    }
//    INCA_DEBUG("Largest contour interval: " << largestInterval)

    // Element 24: Largest contour interval units -- 1 byte => 1 integer
    //      "Corresponds to the units of the map largest primary contour
    //       interval.
    //       Code 0 = Not applicable
    //            1 = feet
    //            2 = meters (level 2 DEMs only)"
    int largestIntervalUnits = readInteger(1, is);
//    INCA_DEBUG("Largest contour interval units: " << largestIntervalUnits)

    // Element 25: Smallest primary contour interval -- 5 bytes => 1 integer
    //      "Smallest or only primary contour interval
    //       (level 2 DEMs only)."
    int smallestInterval = readInteger(5, is);
//    INCA_DEBUG("Smallest contour interval: " << smallestInterval)

    // Element 26: Smallest contour interval units -- 1 byte => 1 integer
    //      "Corresponds to the units of the map smallest primary contour
    //       interval.
    //       Code 1 = feet
    //            2 = meters (level 2 DEMs only)"
    int smallestIntervalUnits = readInteger(1, is);
//    INCA_DEBUG("Smallest contour interval units: " << smallestIntervalUnits)

    // Element 27: Data source date -- 4 bytes => 1 integer
    //      "YYYY 4 character year. The original compilation date and/or
    //       the date of the photography."
    int sourceDate = readInteger(4, is);
//    INCA_DEBUG("Data source date: " << sourceDate)

    // Element 28: Inspection/revision date -- 4 bytes => 1 integer
    //      "YYYY 4 character year. The date of completion and/or the date of
    //       revision."
    int inspectionDate = readInteger(4, is);
//    INCA_DEBUG("Inspection date: " << inspectionDate)

    // Element 29: Inspection flag -- 1 character
    //      "'I' indicates all processes of part 3 (Quality Control) have
    //       been performed."
    char inspCh = readChar(is);
    bool inspected = (inspCh == 'I');
//    INCA_DEBUG("Inspected: " << inspected)

    // Element 30: Data validation flag -- 1 byte => 1 integer
    //      "Code 0 = No validation performed
    //            1 = RMSE computed from test points (record C added), no
    //                quantitative test, no interactive DEM editing or review.
    //            2 = Batch process water body edit and RMSE computer from test
    //                points.
    //            3 = Review and edit, including water edit. No RMSE computed
    //                from test points.
    //            4 = Level 1 DEMs reviewed and edited. Includes water body
    //                editing. RMSE computed from test points.
    //            5 = Level 2 and 3 DEMs reviewed and edited. Includes water
    //                body editing and verification or vertical integration of
    //                planimetric categories (other than hypsography or
    //                hydrography if authorized). RMSE computed from test
    //                points."
    int validationLevel = readInteger(1, is);
//    INCA_DEBUG("Validation level: " << validationLevel)

    // Element 31: Suspect/void flag -- 2 bytes => 1 integer
    //      "Code 0 = None
    //            1 = suspect areas
    //            2 = void areas
    //            3 = suspect and void areas"
    int svFlags = readInteger(2, is);
    bool hasSuspect = (svFlags & 0x01) != 0;
    bool hasVoid = (svFlags & 0x02) != 0;
//    INCA_DEBUG("Has suspect areas: " << hasSuspect)
//    INCA_DEBUG("Has void areas: " << hasVoid)

    // Element 32: Vertical datum -- 2 bytes => 1 integer
    //      "Code 1 = local mean sea-level
    //            2 = National Geodetic Vertical Datum 1929 (NGVD 29)
    //            3 = North American Vertical Datum 1988 (NAVD 88)
    //       See appendix H for datum information."
    int verticalDatum = readInteger(2, is);
//    INCA_DEBUG("Vertical datum: " << verticalDatum)

    // Element 33: Horizontal datum -- 2 bytes => 1 integer
    //      "Code 1 = North American Datum 1927 (NAD 27)
    //            2 = World Geodetic System 1972 (WGS 72)
    //            3 = World Geodetic System 1984 (WGS 84)
    //            4 = North American Datum 1983 (NAD 83)
    //            5 = Old Hawaii Datum
    //            6 = Puerto Rico Datum
    //       See appendix H for datum information."
    int horizontalDatum = readInteger(2, is);
//    INCA_DEBUG("Horizontal datum: " << horizontalDatum)

    // Element 34: Data edition -- 4 bytes => 1 integer
    //      "[01, 99] Primarily a DMA specific field. For USGS use, set to 01.
    int dataEdition = readInteger(4, is);
//    INCA_DEBUG("Data edition: " << dataEdition);

    // Element 35: Percent void -- 4 bytes => 1 integer
    //      "If element [31] indicates [the presence of] void [nodes], this
    //       field (right justified) contains the percentage of nodes in the
    //       file set to void (-32767)."
    int percentVoid = readInteger(4, is);
//    INCA_DEBUG("Percent void: " << percentVoid)

    // Element 36: Edge match flags -- 8 bytes => 4 integers
    //      "Ordered west, north, east, and south. See section 2.2.4 for valid
    //       flags and explanation of codes."
    int edgeMatch[4];
    for (IndexType i = 0; i < 4; i++) {
        edgeMatch[i] = readInteger(2, is);
//        INCA_DEBUG("Edge match " << i << ": " << edgeMatch[i])
    }

    // Element 37: Vertical datum shift -- 7 bytes => 1 real
    //      "Value is in the form of SFFF.DD. Value is the average shift value
    //       for the four quadrangle corners obtained from the program VERTCON.
    //       Always add this value to convert to NAVD 88."
    double verticalShift = readDouble(8, is);
//    INCA_DEBUG("Vertical datum shift: " << verticalShift)
}

// Parse the next B-record (elevation profile) in the file
void DEMInterpreter::parseRecordTypeB(IndexType r, IndexType c, std::istream & is) {
    // Element 1: Row/column identifier -- 12 bytes => 2 integers
    //      "A two-element array containing the row and column identification
    //       number of the DEM profile contained in this record. The row and
    //       column numbers may range from 1 to m and 1 to n. The row number
    //       is normally set to 1. The colum identification is the profile
    //       sequence number."
    std::size_t row = readInteger(6, is);
    std::size_t col = readInteger(6, is);
    if (row != r || col != c) {
        INCA_WARNING("parseRecordTypeB(" << r << ", " << c << "): Incorrect "
                     "profile identifier (" << row << ", " << col << ")")
    } else {
//        INCA_DEBUG(std::setw(5) << row << "," << std::setw(5) << col)
    }

     // Element 2: Number of samples this profile -- 12 bytes => 2 integers
    //      "A two-element array containing the number (m, n) of elevations
    //       in the DEM profile. The first element in the field corresponds to
    //       the number of rows of nodes in this profile. The second element
    //       is set to 1, specifying 1 column per record."
    std::size_t pRows = readInteger(6, is);
    std::size_t pCols = readInteger(6, is);
//    INCA_DEBUG("Contents: " << pRows << " rows and " << pCols << " columns ")

    // Element 3: Planimetric coordinates of start -- 48 bytes => 2 24-byte reals
    //      "A two-element array containing the ground planimetric coordinates
    //       (Xgp, Ygp) of the first elevation in the profile."
    double startX = readDouble(24, is);
    double startY = readDouble(24, is);
//    INCA_DEBUG("Start coordinates: " << startX << ", " << startY)

    // Element 4: Local elevation datum -- 24 bytes => 1 24-byte real
    //      "Elevation of local datum for the profile. The values are in the
    //       units of measure given by data element [15] in logical record
    //       type A."
    double referenceDatum = readDouble(24, is);
//    INCA_DEBUG("Local reference elevation: " << referenceDatum)

    // Element 5: Elevation extrema -- 48 bytes => 2 24-byte reals
    //      "A two-element array of minimum and maximum elevations for the
    //       profile. The values are in the units of measure given by data
    //       element [15] in logical record type A and are the algebraic
    //       result of the method outlined in data element 6 of this record."
    double minElevation = readDouble(24, is);
    double maxElevation = readDouble(24, is);
//    INCA_DEBUG("Elevation extrema: " << minElevation << ", " << maxElevation)

    // Calculation of what grid row the first elevation belongs in. Since
    // these goofy coordinates may result in non-rectangular quadrilaterals,
    // we need to account for the offset that our row may have.
    double dx = startX - extents[0][0];
    double dy = startY - extents[0][1];
    double cos_phi = std::cos(deviationAngle);
    double sin_phi = std::sin(deviationAngle);
    IndexType startCol = IndexType((dx * cos_phi + dy * sin_phi) / resolution[0]);
    IndexType startRow = IndexType((dx * -sin_phi + dy * cos_phi) / resolution[1]);
//    INCA_DEBUG("Start indices: " << startRow << ", " << startCol)

    // Element 6: Elevations -- m * n * 6 bytes => m * n 6-byte integers
    //      "An m x n array of elevations for the profile. Elevations are
    //       expressed in units of resolution. A maximum of six characters are
    //       allowed for each integer elevation value. A value in this array
    //       would be multiplied by the Z spatial resolution (data
    //       element [21], record type A) and added to the [local datum
    //       elevation](data element 3, record type B) to obtain the elevation
    //       for the point. The planimetric ground coordinates of the point
    //       Xgp, Ygp are computed according to the formulas:
    //          Xp = (i - 1) dX     Xgp = Xgo + Xp * cos(phi) - Yp * sin(phi)
    //          Yp = (j - 1) dY     Ygp = Ygo + Xp * sin(phi) + Yp * cos(phi)"

    // Fill in out-of bounds areas below
    for (IndexType thisRow = 0; thisRow < startRow; thisRow++)
        raster(startCol, thisRow) = Heightfield::ElementType(OUT_OF_BOUNDS);

    // Fill in the valid area in the middle
    for (IndexType thisRow = startRow; thisRow < IndexType(startRow + pRows); thisRow++) {
        int val;
        is >> val;            // Formatted integer read
        raster(startCol, thisRow) =
            Heightfield::ElementType(val * resolution[2] + referenceDatum);
    }

    // Fill in out-of bounds areas above
    for (IndexType thisRow = startRow + pRows; thisRow < IndexType(raster.size(1)); thisRow++)
//    for (IndexType thisRow = startRow + pRows; thisRow < IndexType(raster.height()); thisRow++)
        raster(startCol, thisRow) = Heightfield::ElementType(OUT_OF_BOUNDS);
}


// Low-level function to read an arbitrary number of bytes into the buffer
std::size_t DEMInterpreter::readBytes(std::size_t numBytes, std::istream & is) {
    if (numBytes > BUFFER_SIZE - 1) {
        INCA_WARNING("Unable to satisfy request of " << numBytes << " numBytes. "
                     "Go increase the buffer size")

        // Burn off the requested number of bytes
        while (numBytes > BUFFER_SIZE - 1) {
            is.get(buffer.get(), BUFFER_SIZE);
            numBytes -= BUFFER_SIZE - 1;
        }
    }
    is.get(buffer.get(), numBytes + 1);
    std::size_t count = is.gcount();
    if (count != numBytes)
        INCA_WARNING("readBytes(" << numBytes << "): "
                     "Read only " << count << " numBytes..."
                     << (is.eof()  ? " EOF"  : "")
                     << (is.bad()  ? " bad"  : "")
                     << (is.fail() ? " fail" : ""))
    return count;
}

// Interpret a field of a specified size as a boolean
bool DEMInterpreter::readBoolean(std::size_t numBytes, std::istream & is) {
    std::size_t read = readBytes(numBytes, is);
    if (read == numBytes)   return atoi(buffer.get()) != 0;
    else                    return false;
}

// Interpret a field of a specified size as an integer
int DEMInterpreter::readInteger(std::size_t numBytes, std::istream & is) {
    std::size_t read = readBytes(numBytes, is);
    if (read == numBytes)   return atoi(buffer.get());
    else                    return 0;
}

// Interpret a field of a specified size as a double
double DEMInterpreter::readDouble(std::size_t numBytes, std::istream & is) {
    std::size_t read = readBytes(numBytes, is);
    if (read == numBytes)   return atof(buffer.get());
    else                    return 0.0;
}

// Read a char from the file
char DEMInterpreter::readChar(std::istream & is) {
    std::size_t read = readBytes(1, is);
    if (read == 1)          return buffer[0];
    else                    return '\0';
}

// Read a specified number of chars from the file
const char * DEMInterpreter::readChars(std::size_t numBytes, std::istream & is) {
    std::size_t read = readBytes(numBytes, is);
    if (read == numBytes)   return buffer.get();
    else                    return "";
}
