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

// Import class definition
#include "DEMInterpreter.hpp"
using namespace terrainosaurus;

// Import container definitions
#include <vector>


using std::cout;
using std::cerr;
using std::endl;
using std::string;

// How big is our buffer?
const std::size_t DEMInterpreter::BUFFER_SIZE = 41;

// How big is each record type?
const std::size_t DEMInterpreter::CHUNK_SIZE = 1024;


// Constructor
DEMInterpreter::DEMInterpreter(Raster & r)
    : buffer(new char[BUFFER_SIZE]), raster(r), filename(this) { }

// Destructor
DEMInterpreter::~DEMInterpreter() {
    if (file)
        file.close();
}


// Parse the file, calling the appropriate callbacks
void DEMInterpreter::parse() {
    file.open(string(filename).c_str());
    if (! file) {
        cerr << "Error opening \"" << string(filename) << "\"\n";
        return;
    }
    
    // Header information
    parseRecordTypeA();

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
        std::size_t pos = file.tellg();
        while (baseline < pos) {
            // Advance to the next chunk
            baseline += CHUNK_SIZE;
            chunkCount++;
            file.seekg(baseline);

            // See if there's a newline that we must account for
            int i = file.get();
            if (i == '\n') {
                eolCount++;     // Gotcha!
                baseline++;
            } else {
                file.unget();   // Oops! Sorry to disturb you, ma'am
            }
        }

        // Parse the record
        parseRecordTypeB(r, c);
    }

//    parseRecordTypeC();
}

// Parses an A-type (header) record
void DEMInterpreter::parseRecordTypeA() {
    // Element 1: File name -- 40 characters
    //      "The authorized digital cell name followed by a comma, space, and
    //       the two-character State designator(s) separated by hyphens.
    //       Abbreviations for other countries, such as Canada and Mexico,
    //       shall not be represented in the DEM header."
    string fileName = readChars(40);
    cout << "File name: " << fileName << endl;

    // Element 2: Free-format text -- 40 characters
    //      "Free format descriptor field, contains useful information related
    //       to digital processes such as digitizing instrument, photo codes,
    //       slot widths, etc."
    string comment = readChars(40);
    cout << "Comment: " << comment << endl;

    // Element 3: Filler space -- 29 characters
    //      "Blank fill."
    string filler1 = readChars(29);
    if (filler1 != string(29, ' '))
        cout << "Filler: " << filler1 << endl;

    // Element 4: SE geographic corner -- 26 bytes => 2 * (I4 I2 F7.4)
    //      "SE geographic quadrangle corner ordered as:
    //          x = longitude = SDDDMMSS.SSSS
    //          y = latitude  = SDDDMMSS.SSSS
    //       (negative sign (S) right-justified, no leading zeroes,
    //        plus sign (S) implied)."
    string seCorner = readChars(26);
    cout << "SE corner: " << seCorner << endl;

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
    int processCode = readInteger(1);
    cout << "Process code: " << processCode << endl;

    // Element 6: Filler -- 1 byte
    //      "Blank fill."
    string filler2 = readChars(1);
    if (filler2 != " ")
        cout << "Filler: " << filler2 << endl;

    // Element 7: Sectional indicator -- 3 characters
    //      "This code is specific to 30-minute DEMs. Identifies
    //       1:100,000-scale sections. (See appendix 2-I
    //       [of the DEM specification].)"
    string sectionalIndicator = readChars(3);
    cout << "Sectional indicator: " << sectionalIndicator << endl;

    // Element 8: Origin code -- 4 characters
    //      "Free format Mapping Origin Code. Example: MAC, WMC, MCMC, RMMC,
    //       FS, BLM, CONT (contractor), XX (state postal code)."
    string originCode = readChars(4);
    cout << "Origin code: " << originCode << endl;

    // Element 9: DEM level code -- 6 bytes => 1 integer
    //      "Code 1 = DEM-1
    //            2 = DEM-2
    //            3 = DEM-3
    //            4 = DEM-4"
    int demLevel = readInteger(6);
    cout << "DEM level code: " << demLevel << endl;

    // Element 10: Elevation pattern code -- 6 bytes => 1 integer
    //      "Code 1 = regular
    //            2 = random, reserved for future use"
    int elevationPattern = readInteger(6);
    cout << "Elevation pattern: " << elevationPattern << endl;

    // Element 11: Ground planimetric coord. system code -- 6 bytes => 1 integer
    //      "Code 0 = Geographic
    //            1 = UTM
    //            2 = State plane
    //            3 - 20 : see appendix G
    //       Code 0 represents the geographic (latitude/longitude) suystem for
    //       30-minute, 1-degree, and Alaska DEMs. Code 1 represents the
    //       current use of the UTM coordinate system for 7.5-minute DEMs."
    int coordinateSystem = readInteger(6);
    cout << "Coordinate system: " << coordinateSystem << endl;

    // Element 12: Ground planimetric zone code -- 6 bytes => 1 integer
    //      "Codes for State plane and UTM coordinate zones are given in
    //       appendices E and F for 7.5-minute DEMs. Code is set to zero if
    //       element 5 is also set to zero, defining data as geographic."
    int coordinateZone = readInteger(6);
    cout << "Coordinate zone: " << coordinateZone << endl;

    // Element 13: Map projection parameters -- 360 bytes => 15 24-byte reals
    //      "Definition of parameters for various projections is given in
    //       Appendix F. All 15 fields of this element are set to zero and
    //       should be ignored when goegraphic, UTM, or State plane coordinates
    //       are coded in data element [11]."
    double projectionParameters[15];
    for (IndexType i = 0; i < 15; i++) {
        projectionParameters[i] = readDouble(24);
        cout << "Projection parameter " << i << ": "
             << projectionParameters[i] << endl;
    }

    // Element 14: Ground planimetric unit of measure -- 6 bytes => 1 integer
    //      "Code 0 = radians
    //            1 = feet
    //            2 = meters
    //            3 = arc-seconds
    //       Normally set to code 2 for 7.5-minute DEMs. Always set to code
    //       3 for 30-minute, 1-degree and Alaska DEMs."
    int horizontalUnits = readInteger(6);
    cout << "Horizontal units: " << horizontalUnits << endl;

    // Element 15: Elevation unit of measure -- 6 bytes => 1 integer
    //      "Code 1 = feet
    //            2 = meters
    //       Normally code 2 (meters) for 7.5-minute, 30-minute, 1-degree
    //       and Alaska DEMs."
    int verticalUnits = readInteger(6);
    cout << "Vertical units: " << verticalUnits << endl;

    // Element 16: Number of sides in boundary polygon -- 6 bytes => 1 integer
    //      "Set to n = 4."
    int boundaryPolygonSides = readInteger(6);
    cout << "Boundary polygon sides: " << boundaryPolygonSides << endl;

    // Element 17: Quadrangle corner coordinates -- 192 bytes => 4 24-byte reals
    //      "The coordinates of the quadrangle corners are ordered in a
    //       clockwise direction beginning with the southwest corner. The array
    //       is stored as pairs of eastings and northings."
    double corners[4][2];
    for (IndexType i = 0; i < 4; i++) {
        for (IndexType j = 0; j < 2; j++)
            corners[i][j] = readDouble(24);
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
        cout << "Corner " << i << ": " << corners[i][0] << ", " << corners[i][1] << endl;
    }
//    cout << "Min extents: " << extents[0][0] << ", " << extents[0][1] << endl;
//    cout << "Max extents: " << extents[1][0] << ", " << extents[1][1] << endl;
//    std::vector<Handler::Point2D> vertices;
//    for (IndexType i = 0; i < 4; i++)
//        vertices.push_back(Handler::Point2D(corners[i][0], corners[i][1]));
//    handler->trimmingPolygon = Handler::Polygon(vertices);

    // Element 18: Elevation range -- 48 bytes => 2 24-byte reals
    //      "Minimum and maximum elevations for the DEM. The values are in the
    //       unit of measure given by data element [15] in this record and are
    //       the algebraic result of the method outlined in data element 6,
    //       logical record B [forward reference to record type B]."
    double elevationExtrema[2];
    for (IndexType i = 0; i < 2; i++)
        elevationExtrema[i] = readDouble(24);
    cout << "Elevation extrema: " << elevationExtrema[0]
                        << " => " << elevationExtrema[1] << endl;
    

    // Element 19: Deviation angle -- 24 bytes => 1 24-byte real
    //      "Counterclockwise angle (in radians) from the primary axis of
    //       ground planimetric reference to the primary axis of the DEM local
    //       reference system. Set to zero [if aligned] with the coordinate
    //       system specified in element [11]."
    deviationAngle = readDouble(24);
    cout << "Deviation angle: " << deviationAngle << endl;

    // Element 20: Accuracy record -- 6 bytes => 1 boolean
    //      "Accuracy code for elevations.
    //       Code 0 = unknown accuracy
    //            1 = accuracy information is given in logical record type C."
    bool hasAccuracyRecord = readBoolean(6);
    cout << "Has accuracy record: " << hasAccuracyRecord << endl;

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
        resolution[i] = readDouble(12);
        cout << "Resolution " << i << ": " << resolution[i] << endl;
    }
//    handler->xResolution = resolution[0];
//    handler->yResolution = resolution[1];
//    handler->zResolution = resolution[2];

    // Element 22: Grid size -- 12 bytes => 2 integers
    //      "A two element array containing the number of rows and columns
    //       (m, n) of profiles in the DEM. When the row value m is set to
    //       1, the n value describes the number of columns in the DEM file."
    int rows = readInteger(6);
    int columns = readInteger(6);
    double colGuess = (extents[1][0] - extents[0][0]) / resolution[0];
    double rowGuess = (extents[1][1] - extents[0][1]) / resolution[1];
    cout << "Profiles: " << rows << " rows x " << columns << " columns\n";
    cout << "Guess: " << rowGuess << " rows x " << colGuess << " columns\n";
    raster.resize(columns, std::size_t(rowGuess + 1));


    /*************************************************************************
     * At this point, the original specification stops. The 1988 format adds
     * the following additional fields.
     *************************************************************************/


    // Element 23: Largest primary contour interval -- 5 bytes => 1 integer
    //      "Present only if two or more primary intervals exist
    //       (level 2 DEMs only)."
    int largestInterval = readInteger(5);
    if (string(buffer.get()) == "     ") {
        cout << "This DEM appears to follow the pre-1988 format" << endl;
        return;
    }
    cout << "Largest contour interval: " << largestInterval << endl;

    // Element 24: Largest contour interval units -- 1 byte => 1 integer
    //      "Corresponds to the units of the map largest primary contour
    //       interval.
    //       Code 0 = Not applicable
    //            1 = feet
    //            2 = meters (level 2 DEMs only)"
    int largestIntervalUnits = readInteger(1);
    cout << "Largest contour interval units: " << largestIntervalUnits << endl;

    // Element 25: Smallest primary contour interval -- 5 bytes => 1 integer
    //      "Smallest or only primary contour interval
    //       (level 2 DEMs only)."
    int smallestInterval = readInteger(5);
    cout << "Smallest contour interval: " << smallestInterval << endl;

    // Element 26: Smallest contour interval units -- 1 byte => 1 integer
    //      "Corresponds to the units of the map smallest primary contour
    //       interval.
    //       Code 1 = feet
    //            2 = meters (level 2 DEMs only)"
    int smallestIntervalUnits = readInteger(1);
    cout << "Smallest contour interval units: " << smallestIntervalUnits << endl;

    // Element 27: Data source date -- 4 bytes => 1 integer
    //      "YYYY 4 character year. The original compilation date and/or
    //       the date of the photography."
    int sourceDate = readInteger(4);
    cout << "Data source date: " << sourceDate << endl;

    // Element 28: Inspection/revision date -- 4 bytes => 1 integer
    //      "YYYY 4 character year. The date of completion and/or the date of
    //       revision."
    int inspectionDate = readInteger(4);
    cout << "Inspection date: " << inspectionDate << endl;

    // Element 29: Inspection flag -- 1 character
    //      "'I' indicates all processes of part 3 (Quality Control) have
    //       been performed."
    char inspCh = readChar();
    bool inspected = (inspCh == 'I');
    cout << "Inspected: " << inspected << endl;

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
    int validationLevel = readInteger(1);
    cout << "Validation level: " << validationLevel << endl;

    // Element 31: Suspect/void flag -- 2 bytes => 1 integer
    //      "Code 0 = None
    //            1 = suspect areas
    //            2 = void areas
    //            3 = suspect and void areas"
    int svFlags = readInteger(2);
    bool hasSuspect = (svFlags & 0x01) != 0;
    bool hasVoid = (svFlags & 0x02) != 0;
    cout << "Has suspect areas: " << hasSuspect << endl;
    cout << "Has void areas: " << hasVoid << endl;

    // Element 32: Vertical datum -- 2 bytes => 1 integer
    //      "Code 1 = local mean sea-level
    //            2 = National Geodetic Vertical Datum 1929 (NGVD 29)
    //            3 = North American Vertical Datum 1988 (NAVD 88)
    //       See appendix H for datum information."
    int verticalDatum = readInteger(2);
    cout << "Vertical datum: " << verticalDatum << endl;

    // Element 33: Horizontal datum -- 2 bytes => 1 integer
    //      "Code 1 = North American Datum 1927 (NAD 27)
    //            2 = World Geodetic System 1972 (WGS 72)
    //            3 = World Geodetic System 1984 (WGS 84)
    //            4 = North American Datum 1983 (NAD 83)
    //            5 = Old Hawaii Datum
    //            6 = Puerto Rico Datum
    //       See appendix H for datum information."
    int horizontalDatum = readInteger(2);
    cout << "Horizontal datum: " << horizontalDatum << endl;

    // Element 34: Data edition -- 4 bytes => 1 integer
    //      "[01, 99] Primarilya DMA specific field. For USGS use, set to 01.
    int dataEdition = readInteger(4);
    cout << "Data edition: " << dataEdition << endl;

    // Element 35: Percent void -- 4 bytes => 1 integer
    //      "If element [31] indicates [the presence of] void [nodes], this
    //       field (right justified) contains the percentage of nodes in the
    //       file set to void (-32767)."
    int percentVoid = readInteger(4);
    cout << "Percent void: " << percentVoid << endl;

    // Element 36: Edge match flags -- 8 bytes => 4 integers
    //      "Ordered west, north, east, and south. See section 2.2.4 for valid
    //       flags and explanation of codes."
    int edgeMatch[4];
    for (IndexType i = 0; i < 4; i++) {
        edgeMatch[i] = readInteger(2);
        cout << "Edge match " << i << ": " << edgeMatch[i] << endl;
    }

    // Element 37: Vertical datum shift -- 7 bytes => 1 real
    //      "Value is in the form of SFFF.DD. Value is the average shift value
    //       for the four quadrangle corners obtained from the program VERTCON.
    //       Always add this value to convert to NAVD 88."
    double verticalShift = readDouble(8);
    cout << "Vertical datum shift: " << verticalShift << endl;
}

// Parse the next B-record (elevation profile) in the file
void DEMInterpreter::parseRecordTypeB(IndexType r, IndexType c) {
    // Element 1: Row/column identifier -- 12 bytes => 2 integers
    //      "A two-element array containing the row and column identification
    //       number of the DEM profile contained in this record. The row and
    //       column numbers may range from 1 to m and 1 to n. The row number
    //       is normally set to 1. The colum identification is the profile
    //       sequence number."
    std::size_t row = readInteger(6);
    std::size_t col = readInteger(6);
    if (row != r || col != c) {
        std::ostringstream ss;
        ss << "parseRecordTypeB(" << r << ", " << c << "): Incorrect "
               "profile identifier (" << row << ", " << col << ")";
        warn(ss.str());
//        return;
    } else {
//        cerr << std::setw(5) << row << "," << std::setw(5) << col << endl;
    }

     // Element 2: Number of samples this profile -- 12 bytes => 2 integers
    //      "A two-element array containing the number (m, n) of elevations
    //       in the DEM profile. The first element in the field corresponds to
    //       the number of rows of nodes in this profile. The second element
    //       is set to 1, specifying 1 column per record."
    std::size_t pRows = readInteger(6);
    std::size_t pCols = readInteger(6);
//    cout << "Contents: " << pRows << " rows and " << pCols << " columns " << endl;

    // Element 3: Planimetric coordinates of start -- 48 bytes => 2 24-byte reals
    //      "A two-element array containing the ground planimetric coordinates
    //       (Xgp, Ygp) of the first elevation in the profile."
    double startX = readDouble(24);
    double startY = readDouble(24);
//    cout << "Start coordinates: " << startX << ", " << startY << endl;

    // Element 4: Local elevation datum -- 24 bytes => 1 24-byte real
    //      "Elevation of local datum for the profile. The values are in the
    //       units of measure given by data element [15] in logical record
    //       type A."
    double referenceDatum = readDouble(24);
//    cout << "Local reference elevation: " << referenceDatum << endl;

    // Element 5: Elevation extrema -- 48 bytes => 2 24-byte reals
    //      "A two-element array of minimum and maximum elevations for the
    //       profile. The values are in the units of measure given by data
    //       element [15] in logical record type A and are the algebraic
    //       result of the method outlined in data element 6 of this record."
    double minElevation = readDouble(24);
    double maxElevation = readDouble(24);
//    cout << "Elevation extrema: " << minElevation << ", " << maxElevation << endl;

    // Calculation of what grid row the first elevation belongs in. Since
    // these goofy coordinates may result in non-rectangular quadrilaterals,
    // we need to account for the offset that our row may have.
    double dx = startX - extents[0][0];
    double dy = startY - extents[0][1];
    double cos_phi = cos(deviationAngle);
    double sin_phi = sin(deviationAngle);
    IndexType startCol = IndexType((dx * cos_phi + dy * sin_phi) / resolution[0]);
    IndexType startRow = IndexType((dx * -sin_phi + dy * cos_phi) / resolution[1]);
//    cout << "Start indices: " << startRow << ", " << startCol << endl;

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
        raster(startCol, thisRow) = float(OUT_OF_BOUNDS);
    
    // Fill in the valid area in the middle
    for (IndexType thisRow = startRow; thisRow < IndexType(startRow + pRows); thisRow++) {
        int val;
        file >> val;            // Formatted integer read
        raster(startCol, thisRow) = float(val * resolution[2] + referenceDatum);
    }

    // Fill in out-of bounds areas above
    for (IndexType thisRow = startRow + pRows; thisRow < IndexType(raster.size(1)); thisRow++)
//    for (IndexType thisRow = startRow + pRows; thisRow < IndexType(raster.height()); thisRow++)
        raster(startCol, thisRow) = float(OUT_OF_BOUNDS);
}


// Low-level function to read an arbitrary number of bytes into the buffer
std::size_t DEMInterpreter::readBytes(std::size_t numBytes) {
    if (numBytes > BUFFER_SIZE - 1) {
        // Complain to the model handler
        std::ostringstream ss;
        ss << "Unable to satisfy request of " << numBytes << " numBytes. "
              "Go increase the buffer size";
        warn(ss.str());

        // Burn off the requested number of bytes
        while (numBytes > BUFFER_SIZE - 1) {
            file.get(buffer.get(), BUFFER_SIZE);
            numBytes -= BUFFER_SIZE - 1;
        }
    }
    file.get(buffer.get(), numBytes + 1);
    std::size_t count = file.gcount();
    if (count != numBytes) {
        std::ostringstream ss;
        ss << "readBytes(" << numBytes << "): Read only " << count << " numBytes...";
        if (file.eof())     ss << " EOF";
        if (file.bad())     ss << " bad";
        if (file.fail())    ss << " fail";
        warn(ss.str());
    }
    return count;
}

// Interpret a field of a specified size as a boolean
bool DEMInterpreter::readBoolean(std::size_t numBytes) {
    std::size_t read = readBytes(numBytes);
    if (read == numBytes)   return atoi(buffer.get()) != 0;
    else                    return false;
}

// Interpret a field of a specified size as an integer
int DEMInterpreter::readInteger(std::size_t numBytes) {
    std::size_t read = readBytes(numBytes);
    if (read == numBytes)   return atoi(buffer.get());
    else                    return 0;
}

// Interpret a field of a specified size as a double
double DEMInterpreter::readDouble(std::size_t numBytes) {
    std::size_t read = readBytes(numBytes);
    if (read == numBytes)   return atof(buffer.get());
    else                    return 0.0;
}

// Read a char from the file
char DEMInterpreter::readChar() {
    std::size_t read = readBytes(1);
    if (read == 1)          return buffer[0];
    else                    return '\0';
}

// Read a specified number of chars from the file
const char * DEMInterpreter::readChars(std::size_t numBytes) {
    std::size_t read = readBytes(numBytes);
    if (read == numBytes)   return buffer.get();
    else                    return "";
}
