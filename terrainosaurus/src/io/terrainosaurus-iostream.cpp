/*
 * File: terrainosaurus-iostream.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Import function prototypes
#include "terrainosaurus-iostream.hpp"

// Import parser definitions
#include "TerrainLibraryParser.hpp"
#include "TerrainLibraryLexer.hpp"
#include "MapParser.hpp"
#include "MapLexer.hpp"
#include <antlr/TokenStreamException.hpp>

// Import file-related exception definitions
#include <inca/io/FileExceptions.hpp>

using namespace inca::io;
using namespace std;


// IOstream operators for (de)serializing instances of TerrainLibrary
istream & terrainosaurus::operator>>(istream &is, TerrainLibrary &tl) {
    try {
        // Attempt to parse the stream
        TerrainLibraryLexer lexer(is);
        TerrainLibraryParser parser(lexer);
        parser.recordList(&tl);

    // Other ANTLR parsing exception
    } catch (const antlr::RecognitionException &e) {
        FileFormatException ffe(e.getFilename(),
                                e.getLine(), e.getColumn(),
                                e.getMessage());
        throw ffe;

    // ANTLR IO exception
    } catch (const antlr::TokenStreamException &e) {
        FileFormatException ffe("", 0, 0, e.getMessage());
        throw ffe;
    }

    return is;
}

ostream & terrainosaurus::operator<<(ostream &os, const TerrainLibrary &tl) {
    // First, write out each TerrainType record
    const TerrainLibrary::TerrainTypeList & types = tl.terrainTypes();
    for (IndexType i = 0; i < IndexType(types.size()); ++i) {
        const TerrainType & tt = *types[i];
        os << "[TerrainType: " << tt.name() << "]\n"
           << "Color = " <<  tt.color() << '\n'
           << '\n';
    }

    // Now, write out each unique TerrainSeam record
    const TerrainLibrary::TerrainSeamMatrix &seams = tl.terrainSeams();
    for (IndexType i = 0; i < IndexType(seams.size()); ++i)
        for (IndexType j = 0; j < i; ++j) {
            const TerrainSeam & ts = *seams[i][j];    // Pick out the seam
            os << "[TerrainSeam: " << types[ts.terrainType1()]->name()
                   << " & " << types[ts.terrainType2()]->name() << "]\n"
               << "Number of Chromosomes = " << ts.numberOfChromosomes() << '\n'
               << "Smoothness = " << ts.smoothness() << '\n'
               << "Mutation Ratio = " << ts.mutationRatio() << '\n'
               << "Crossover Ratio = " << ts.crossoverRatio() << '\n'
               << "Selection Ratio = " << ts.selectionRatio() << '\n'
               << "Aspect Ratio = " << ts.aspectRatio() << '\n'
               << '\n';
        }

    return os;
}


// IOstream operators for (de)serializing instances of Map
istream & terrainosaurus::operator>>(istream &is, Map &m) {
    MapLexer lexer(is);
    MapParser parser(lexer);

    try {
        // Attempt to parse the stream
        parser.terrainMap(&m);

    // Other ANTLR parsing exception
    } catch (const antlr::RecognitionException &e) {
        FileFormatException ffe(parser.getFilename(),
                                e.getLine(), e.getColumn(),
                                e.getMessage());
        throw ffe;

    // ANTLR IO exception
    } catch (const antlr::TokenStreamException &e) {
        FileFormatException ffe(parser.getFilename(), 0, 0, e.getMessage());
        throw ffe;
    }

    return is;
}

ostream & terrainosaurus::operator<<(ostream &os, const Map &m) {
    // Create the file header comments
    os << "# This file is a Terrainosaurus map, generated by the MapExplorer "
          "tool.\n"
       << '\n';

    // Write out the vertices (intersections)
    os << "# Vertex declarations\n"
       << '\n';

    // Write out the faces (regions)
    os << "# Vertex declarations\n"
       << '\n';

    return os;
}
