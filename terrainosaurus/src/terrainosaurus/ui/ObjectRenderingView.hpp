/*
 * File: ObjectRenderingView.hpp
 *
 * Author: Ryan L. Saunders
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      FIXME: This class needs a different name. This is sorta just a hack until I figure out the "right" way to do this.
 */

#ifndef TERRAINOSAURUS_UI_OBJECT_RENDERING_VIEW
#define TERRAINOSAURUS_UI_OBJECT_RENDERING_VIEW

// Import Inca library configuration
#include <terrainosaurus/terrainosaurus-common.h>

// This is part of the Terrainosaurus terrain generation engine
namespace terrainosaurus {
    // Forward declarations
    template <class T> class ObjectRenderingView;
};


// Import superclass definition
#include <inca/ui/View.hpp>


template <class T>
class terrainosaurus::ObjectRenderingView : public inca::ui::View {
public:
    typedef T                   Object;
    typedef shared_ptr<T>       ObjectPtr;
    typedef shared_ptr<T const> ObjectConstPtr;

    // Constructor
    ObjectRenderingView() { }
    ObjectRenderingView(Object * ptr)  : _object(ptr) { }
    ObjectRenderingView(ObjectPtr ptr) : _object(ptr) { }

    ObjectPtr      object()       { return _object; }
    ObjectConstPtr object() const { return _object; }
    void setObject(ObjectPtr ptr) {
        _object = ptr;
    }

    void initializeView() { }
    void resizeView(Dimension size) { }
    void renderView() {
        if (_object)
            (*_object)(this->renderer());
    }

protected:
    ObjectPtr _object;
};

#endif
