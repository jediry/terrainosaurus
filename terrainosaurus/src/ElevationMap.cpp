/*
 * File: ElevationMap.cpp
 * 
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. All rights reserved.
 *
 * Description:
 *      The ElevationMap class is an object wrapper around a
 *      "heightfield", a 3D object based on a 2D data set giving
 *      the height at regular intervals.
 */

// Import class definition
#include "ElevationMap.hpp"
using namespace Terrainosaurus;


// Sentinel value definitions
const double ElevationMap::OUT_OF_BOUNDS = -1.7e-300;
const double ElevationMap::VOID          = -1.6e-300;


// Constructor
ElevationMap::ElevationMap() {
    MaterialPtr mat(new Material());
    mat->diffuse = Material::Color(0.0f, 0.6f, 0.1f);
    mat->specular = Material::Color(0.1f, 0.1f, 0.1f);
    mat->illuminated = true;
    addMaterial(mat);
}

// Create a tessellation of this surface
void ElevationMap::updateTessellation(const Point &viewPoint,
                                      const Vector &look) {
    if (tessellation->isDirty()) {
        tessellation->clear();
        tessellation->setGroupCount(1);
        Tessellation::TriangleGroup &group = tessellation->triangleGroup(0);
        tessellation->setHasNormals(0, true);
        tessellation->setHasTexCoords(0, false);
        const ElevationMap & thisMap = *this;

        Tessellation::Triangle tri;
        Transform::scalar_t col_2 = columns() / 2.0,
                            row_2 = rows() / 2.0;
        for (index_t r = 0; r < rows()  - 1; r++)
            for (index_t c = 0; c < columns() - 1; c++) {
                if (thisMap(r, c) != OUT_OF_BOUNDS
                        && thisMap(r + 1, c) != OUT_OF_BOUNDS
                        && thisMap(r, c + 1) != OUT_OF_BOUNDS
                        && thisMap(r + 1, c + 1) != OUT_OF_BOUNDS) {
                    //Point p1(r - row_2, 0, c - col_2);
                    //Point p2(r - row_2, 0, c + 1 - col_2);
                    //Point p3(r + 1 - row_2, 0, c + 1 - col_2);
                    //Point p4(r + 1 - row_2, 0, c - col_2);
                    Point p1(r - row_2, thisMap(r, c), c - col_2);
                    Point p2(r - row_2, thisMap(r, c + 1), c + 1 - col_2);
                    Point p3(r + 1 - row_2, thisMap(r + 1, c + 1), c + 1 - col_2);
                    Point p4(r + 1 - row_2, thisMap(r + 1, c), c - col_2);
                    Vector n1 = normalize((p2 - p1) % (p3 - p1));
                    Vector n2 = normalize((p3 - p1) % (p4 - p1));
                    tri.v[0] = tessellation->addVertex(p1);
                    tri.n[0] = tessellation->addNormal(n1);
                    tri.v[1] = tessellation->addVertex(p2);
                    tri.n[1] = tri.n[0];
                    tri.v[2] = tessellation->addVertex(p3);
                    tri.n[2] = tri.n[0];
                    group.push_back(tri);
                    tri.n[0] = tessellation->addNormal(n2);
                    tri.v[1] = tri.v[2];
                    tri.n[1] = tri.n[0];
                    tri.v[2] = tessellation->addVertex(p4);
                    tri.n[2] = tri.n[0];
                    group.push_back(tri);
                }
            }
    }
}
