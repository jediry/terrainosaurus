/*
 * File: MapSelection.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MapSelection class represents a selection of items in a map.
 *      It inherits all of the set operations of the Inca library's selection,
 *      and adds the characteristic that it can represent a selection of the
 *      different types of elements contained in a map: Regions, Intersections,
 *      and Boundaries (only one type of element may be selected at a time).
 *      Essentially, a MapSelection is a sort of set that can switch between
 *      different element types.
 */

#ifndef TERRAINOSAURUS_MAP_SELECTION
#define TERRAINOSAURUS_MAP_SELECTION

// Import library configuration
#include "terrainosaurus-common.h"

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapSelection;
    
    // Pointer typedefs
    typedef shared_ptr<MapSelection>       MapSelectionPtr;
    typedef shared_ptr<MapSelection const> MapSelectionConstPtr;
};

// Import superclass definition
#include <inca/world/SelectionSet.hpp>

// Import Map definition
#include "Map.hpp"


class terrainosaurus::MapSelection : public inca::world::SelectionSet {
private:
    // Set up this object to own properties
    PROPERTY_OWNING_OBJECT(MapSelection);

public:
    // Enumeration of the different types of elements that may be selected
    enum ElementType {
        Regions,        // We're selecting Regions!
        Boundaries,     // No, wait! We're selecting Boundaries!
        Intersections,  // No, you're both wrong. It's Intersections!
    };


/*---------------------------------------------------------------------------*
 | Constructors & data
 *---------------------------------------------------------------------------*/
public:
    // Constructor taking the Map whose parts are to be selected
    MapSelection() : map(this), type(Regions) { }

    // Assignment operator XXX HACK XXX
    MapSelection & operator=(MapSelection &ms);

    // The map we're selecting things from
    rw_ptr_property(Map, map, NULL);

    // The type of element we're currently selecting
    ElementType elementType() const { return type; }
    void setElementType(ElementType t); // Pick element type (clears selection)

protected:
    ElementType type;       // The type of elements that this we're selecting


/*---------------------------------------------------------------------------*
 | Selected element functions
 *---------------------------------------------------------------------------*/
public:
    // Count functions
    size_t selectedCount() const;       // How many elements are selected?
    size_t unselectedCount() const;     // How many elements are not selected?

    // Region accessor functions
    size_t selectedRegionCount() const;     // How many Regions are selected?
    size_t unselectedRegionCount() const;   // How many not?
    void select(const Map::Region &r);      // Make sure this is selected
    void deselect(const Map::Region &r);    // Make sure this is not selected
    bool isSelected(const Map::Region &r) const;    // Is this selected?

    // Boundary accessor functions
    size_t selectedBoundaryCount() const;
    size_t unselectedBoundaryCount() const;
    void select(const Map::Boundary &b);
    void deselect(const Map::Boundary &b);
    bool isSelected(const Map::Boundary &b) const;

    // Intersection accessor functions
    size_t selectedIntersectionCount() const;
    size_t unselectedIntersectionCount() const;
    void select(const Map::Intersection &i);
    void deselect(const Map::Intersection &i);
    bool isSelected(const Map::Intersection &i) const;
};

#endif