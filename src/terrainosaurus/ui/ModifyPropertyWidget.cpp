/*
 * File: ModifyPropertyWidget.cpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */

// Include precompiled header
#include <terrainosaurus/precomp.h>

// Import class definition
#include "ModifyPropertyWidget.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;


/*---------------------------------------------------------------------------*
 | Constructor & properties
 *---------------------------------------------------------------------------*/
// Default constructor with optional component name
ModifyPropertyWidget::ModifyPropertyWidget(const string &nm)
        : MapEditWidget(nm) { }


/*---------------------------------------------------------------------------*
 | Widget event-handler functions
 *---------------------------------------------------------------------------*/
void ModifyPropertyWidget::keyPressed(KeyCode key, Pixel p) {
    switch (key) {
        // R and U refine and unrefine the selected edges, or the entire
        // map, if nothing is selected
        case KeyR:      // 'R' == Refine selection/map
            if (theseModifiersActive(NoModifiers)) {
                refineSelectedEdges(true);
                requestRedisplay();
                return;
            }
            break;
        case KeyU:      // 'U' == Unrefine selection/map
            if (theseModifiersActive(NoModifiers)) {
                refineSelectedEdges(false);
                requestRedisplay();
                return;
            }
            break;

        // Plus/minus keys increase/decrease number of GA cycles for selected
        // boundaries
        case KeyPlus:   // '+' == Increase number of GA iterations
            if (theseModifiersActive(NoModifiers)) {
                adjustSelectedEdgesGAIterations(1);
                requestRedisplay();
                return;
            }
            break;
        case KeyMinus:  // '-' == Decrease number of GA iterations
            if (theseModifiersActive(NoModifiers)) {
                adjustSelectedEdgesGAIterations(-1);
                requestRedisplay();
                return;
            }
            break;

        // Number keys set the terrain type for the selected Regions, or do
        // nothing if no regions are selected.
        case Key1: case Key2: case Key3: case Key4: case Key5:
        case Key6: case Key7: case Key8: case Key9: case Key0:
            if (theseModifiersActive(NoModifiers)) {
                setSelectedFacesTerrainType(1 + key - Key1);   // Skip TT zero
                requestRedisplay();
                return;
            }
            break;
    }
    
    // If we got here, this message isn't for us. Move along. Move along.
    MapEditWidget::keyPressed(key, p);
}


/*---------------------------------------------------------------------------*
 | High-level functions (called from event-handlers)
 *---------------------------------------------------------------------------*/
void ModifyPropertyWidget::refineSelectedEdges(bool refined) {
    // Determine which Edges are selected
    MeshSelection edges;
    edges.getEffectiveSelection(*persistentSelection(), MeshSelection::Edges);

    // If we don't have any Boundaries, go ahead and select the whole map
    if (edges.size() == 0)
        edges.selectAll();

    // (Un)refine each selected Boundary sequentially
    Map &m = *map();
    MeshSelection::iterator i;
    if (refined)
        for (i = edges.begin(); i != edges.end(); ++i)
            m.edge(*i)->refine();
    else
        for (i = edges.begin(); i != edges.end(); ++i)
            m.edge(*i)->unrefine();
}

void ModifyPropertyWidget::adjustSelectedEdgesGAIterations(int deltaCycles) {

}

void ModifyPropertyWidget::setSelectedFacesTerrainType(unsigned int tt) {
    // If the requested TerrainType index is out of range, complain
    if (tt < 0 || tt >= terrainLibrary()->size()) {
        cerr << "Refusing to set out-of-range TerrainType index " << tt << endl;
        return;
    }

    // Get the actual TT object type and set it
    TerrainTypePtr ttp = terrainLibrary()->terrainType(tt);
    setSelectedFacesTerrainType(ttp);
}

void ModifyPropertyWidget::setSelectedFacesTerrainType(TerrainTypePtr tt) {
    // Determine which Faces are selected
    MeshSelection faces;
    faces.getEffectiveSelection(*persistentSelection(), MeshSelection::Edges);

    // Set the TerrainType for each selected Region sequentially
    MeshSelection::iterator i;
    Map &m = *map();
    for (i = faces.begin(); i != faces.end(); ++i)
        m.face(*i)->setTerrainType(tt);
}
