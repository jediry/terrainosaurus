/*
 * File: terrainosaurus-common.h
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2003, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file acts as the central configuration point for the
 *      Terrainosaurus terrain generation/visualization engine. Its job is to
 *      include external libraries (e.g., C++ STL, Inca) that should be
 *      considered "global" to the Terrainosaurus source. It also may include
 *      Terrainosaurus-global definitions and constants.
 */
 
#ifndef TERRAINOSAURUS_COMMON
#define TERRAINOSAURUS_COMMON

// Import Inca library configuration
#include <inca-common.h>
#include <inca-config-math.h>
#include <inca-config-imaging.h>
//#include <inca-config-dynamics.h>

#include <world/World.h++>
#include <poly/Poly.h++>
#include <rendering/Rendering.h++>
#include <io/IOUtilities.h++>

#include <interface/generic/Framework.h++>
#include <interface/generic/CameraControl.h++>
#include <interface/generic/SceneView.h++>
#include <interface/glut/GLUTFramework.h++>

// Import namespaces
using namespace Inca;
using namespace Inca::Math;
using namespace Inca::World;
using namespace Inca::Poly;
using namespace Inca::Interface;
using namespace Inca::Rendering;

#endif
