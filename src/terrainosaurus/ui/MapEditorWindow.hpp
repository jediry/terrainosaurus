/** -*- C++ -*-
 *
 * \file    MapEditorWindow.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      The MapEditorWindow class is a top-level Window subclass for
 *      displaying and editing 2D maps.
 */

#pragma once
#ifndef TERRAINOAURUS_UI_MAP_EDITOR_WINDOW
#define TERRAINOAURUS_UI_MAP_EDITOR_WINDOW

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    class MapEditorWindow;

    // Pointer typedefs
    typedef shared_ptr<MapEditorWindow>       MapEditorWindowPtr;
    typedef shared_ptr<MapEditorWindow const> MapEditorWindowConstPtr;
}

// Import superclass definition
#include <inca/ui/Window.hpp>

// Import data object definitions
#include <terrainosaurus/data/Map.hpp>
#include <terrainosaurus/data/MeshSelection.hpp>


// Simple window displaying an image
class terrainosaurus::MapEditorWindow : public inca::ui::Window {
public:
    // Constructor taking a map object to edit
    MapEditorWindow(MapPtr m, const std::string & nm = std::string());
    MapEditorWindow(MapPtr m, MeshSelectionPtr ps, MeshSelectionPtr ts,
                    const std::string & nm = std::string());
                    
    // Second-phase initialization
    void construct();

    // Map accessors    
    MapPtr      map();
    MapConstPtr map() const;
    void setMap(MapPtr m);
    
    // Persistent selection accessors
    MeshSelectionPtr      persistentSelection();
    MeshSelectionConstPtr persistentSelection() const;
    void setPersistentSelection(MeshSelectionPtr s);
    
    // Transient selection accessors
    MeshSelectionPtr      transientSelection();
    MeshSelectionConstPtr transientSelection() const;
    void setTransientSelection(MeshSelectionPtr s);

protected:
    // References to shareable data objects
    MapPtr              _map;
    MeshSelectionPtr    _persistentSelection,
                        _transientSelection;
};

#endif
