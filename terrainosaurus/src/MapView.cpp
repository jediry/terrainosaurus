#include "MapView.hpp"
using namespace terrainosaurus;
using namespace inca::rendering;
using namespace inca::ui;

MapView::MapView(const string &nm)
    : View(nm), map(this), camera(this),
      terrainColor(this), backgroundColor(this), intersectionColor(this),
      boundaryColor(this), refinedBoundaryColor(this), gridColor(this),
      selectionColor(this),
      boundaryWidth(this), refinedBoundaryWidth(this), intersectionSize(this),
      renderRegions(this), renderIntersections(this), renderBoundaries(this),
      renderRefinedBoundaries(this), renderGrid(this) {

    // Create the list of terrain-type colors
    size_t count = 10;
    for (index_t i = 0; i < count; i++) {
        float shade = 1.0f - (float(i) / count * 0.5f);
        terrainColor.add(Color(shade, shade, shade, 1.0f));
    }
}

/*---------------------------------------------------------------------------*
 | Rendering functions
 *---------------------------------------------------------------------------*/
void MapView::initializeView() {

}

void MapView::resizeView(Dimension d) {
    camera->reshape(d[0], d[1]);
    renderer.resetCameraProjection();
    renderer.applyCameraProjection(camera());
}

void MapView::renderView() {
    renderer.beginRenderPass(Opacity);
        renderMap(Opacity);
    renderer.endRenderPass();
}

void MapView::renderMap(RenderPass pass) {
    if (map) {
        // Clear the framebuffer
        renderer.clear();

        // Transform to the camera's location
        renderer.resetTransform();
        renderer.applyCameraTransform(camera());
        
        // Transform the map object
    //    renderer.applyTransform(mapObject->transform());

        // Render the map regions 
        if (renderRegions) {
            const Map::RegionList &rs = map()->regions();
            Map::RegionList::const_iterator it;

            for (it = rs.begin(); it != rs.end(); it++) {
                Map::Region r = *it;

                // Possibly overlay the default color with the selection color
                if (isSelected(r))
                    renderer.setColor(terrainColor[r.terrainType()]
                                      % selectionColor());
                else
                    renderer.setColor(terrainColor[r.terrainType()]);

                // Draw the region
                renderer.beginRenderImmediate(Polygon);
                    for (index_t i = 0; i < index_t(r.intersectionCount()); i++)
                        renderer.renderVertex(r.intersection(i).location());
                renderer.endRenderImmediate();
            }
        }

        // Render the unrefined map boundaries
        if (renderBoundaries) {
            const Map::BoundaryList &bs = map()->boundaries();
            Map::BoundaryList::const_iterator it;

            const Color & defaultColor = intersectionColor();
            const Color   selectedColor = defaultColor % selectionColor();
            renderer.setLineWidth(boundaryWidth());

            renderer.beginRenderImmediate(Lines);
            for (it = bs.begin(); it != bs.end(); it++) {
                Map::Boundary b = *it;

                // Possibly overlay the default color with the selection color
                renderer.setColor(isSelected(b) ? selectedColor : defaultColor);

                // Render the line
                renderer.renderVertex(b.startIntersection().location());
                renderer.renderVertex(b.endIntersection().location());
            }
            renderer.endRenderImmediate();
        }

        // Render the refined boundaries
        if (renderRefinedBoundaries) {
            const Map::BoundaryList &bs = map()->boundaries();
            Map::BoundaryList::const_iterator it;
            renderer.setColor(refinedBoundaryColor());
            renderer.setLineWidth(refinedBoundaryWidth());

            for (it = bs.begin(); it != bs.end(); it++) {
                if (it->isRefined()) {
                    Map::RefinedBoundary rb = it->refinement();
                    renderer.beginRenderImmediate(LineStrip);
                        Map::PointList::const_iterator pt;
                        const Map::PointList & points = rb.points();
                        for (pt = points.begin(); pt != points.end(); pt++)
                            renderer.renderVertex(*pt);
                    renderer.endRenderImmediate();
                }
            }
        }

        // Render the map boundary intersections
        if (renderIntersections) {
            const Map::IntersectionList &is = map()->intersections();
            Map::IntersectionList::const_iterator it;
            renderer.setColor(intersectionColor());
            renderer.setPointSize(intersectionSize());

            const Color & defaultColor = intersectionColor();
            const Color   selectedColor = defaultColor % selectionColor();

            renderer.beginRenderImmediate(Points);
            for (it = is.begin(); it != is.end(); it++) {
                Map::Intersection i = *it;
                
                // Possibly overlay the default color with the selection color
                renderer.setColor(isSelected(i) ? selectedColor : defaultColor);

                // Render the intersection
                renderer.renderVertex(i.location());
            }
            renderer.endRenderImmediate();
        }

        // Render a regular grid over the window
        if (renderGrid) {
            Point ps, pe;
            renderer.setColor(gridColor());
            renderer.beginRenderImmediate(Lines);

            // Draw horizontal grid lines
            
            // Draw vertical grid lines

            renderer.endRenderImmediate();
        }
    }
}

bool MapView::isSelected(const Intersection &i) const {
    return false;
}

bool MapView::isSelected(const Region &r) const {
    return false;
}

bool MapView::isSelected(const Boundary &b) const {
    return false;
}

