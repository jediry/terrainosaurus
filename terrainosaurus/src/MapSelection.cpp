#include "MapSelection.hpp"
using namespace terrainosaurus;

MapSelection & MapSelection::operator=(MapSelection &ms) {
    map = ms.map();     // Make sure we're talkin' bout the same map
    items = ms.items;   // Copy over set
    type = ms.type;     // And the element type
    return *this;
}

void MapSelection::setElementType(ElementType t) {
    if (t != type) {    // If we actually need to make a change
        type = t;       // Change the element type
        clear();        // And start with an empty selection
    }
}

size_t MapSelection::selectedCount() const { return size(); }
size_t MapSelection::unselectedCount() const {
    switch (elementType()) {
    case Regions:           return map->regionCount() - size();
    case Boundaries:        return map->boundaryCount() - size();
    case Intersections:     return map->intersectionCount() - size();
    default:
        cerr << "Oops! MapSelection::unselectedCount -- unrecognized: "
                "ElementType " << int(elementType()) << endl;
        return 0;
    }
}

// Region accessor functions
size_t MapSelection::selectedRegionCount() const {
    if (elementType() == Regions)       return size();
    else                                return 0;
}
size_t MapSelection::unselectedRegionCount() const {
    if (elementType() == Regions)       return map->regionCount() - size();
    else                                return map->regionCount();
}
void MapSelection::select(const Map::Region &r) {
    setElementType(Regions);            // Change the element type if needed
    SelectionSet::select(r.id());       // Add this to the selection
}
void MapSelection::deselect(const Map::Region &r) {
    if (elementType() == Regions)       // It can only be selected if it's of the
        SelectionSet::deselect(r.id()); // current element type
}
bool MapSelection::isSelected(const Map::Region &r) const {
    return (elementType() == Regions && SelectionSet::isSelected(r.id()));
}


// Boundary accessor functions
size_t MapSelection::selectedBoundaryCount() const {
    if (elementType() == Boundaries)    return size();
    else                                return 0;
}
size_t MapSelection::unselectedBoundaryCount() const {
    if (elementType() == Boundaries)    return map->boundaryCount() - size();
    else                                return map->boundaryCount();
}
void MapSelection::select(const Map::Boundary &b) {
    setElementType(Boundaries);         // Change the element type if needed
    SelectionSet::select(b.id());       // Add this to the selection
}
void MapSelection::deselect(const Map::Boundary &b) {
    if (elementType() == Boundaries)    // It can only be selected if it's of the
        SelectionSet::deselect(b.id()); // current element type
}
bool MapSelection::isSelected(const Map::Boundary &b) const {
    return (elementType() == Boundaries && SelectionSet::isSelected(b.id()));
}


// Intersection accessor functions
size_t MapSelection::selectedIntersectionCount() const {
    if (elementType() == Intersections) return size();
    else                                return 0;
}
size_t MapSelection::unselectedIntersectionCount() const {
    if (elementType() == Intersections) return map->intersectionCount() - size();
    else                                return map->intersectionCount();
}
void MapSelection::select(const Map::Intersection &i) {
    setElementType(Intersections);      // Change the element type if needed
    SelectionSet::select(i.id());       // Add this to the selection
}
void MapSelection::deselect(const Map::Intersection &i) {
    if (elementType() == Intersections) // It can only be selected if it's of the
        SelectionSet::deselect(i.id()); // current element type
}
bool MapSelection::isSelected(const Map::Intersection &i) const {
    return (elementType() == Intersections && SelectionSet::isSelected(i.id()));
}
