# Process this file with GNU autoconf to produce a configure script.


###############################################################################
##
## Build tools configuration
##
###############################################################################

# Autoconf/Automake configuration
AC_PREREQ(2.53)
AC_INIT(terrainosaurus, 0.2.0, rls@tamu.edu)
AC_CONFIG_AUX_DIR(config)
#AC_CONFIG_MACRO_DIR(./ac_tools)
AC_CONFIG_FILES(Makefile src/Makefile)
AC_CONFIG_SRCDIR(src/terrainosaurus/terrainosaurus-common.h)
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
AM_CONFIG_HEADER(src/config-ac.h)
AC_CANONICAL_HOST


# Compilers and other tools
AC_PROG_CXX         # The C++ compiler
AC_PROG_INSTALL     # The 'install' script
AC_PROG_RANLIB      # Tool for building static libraries
#AC_PROG_LIBTOOL     # Tool for building dynamic libraries

# Compiler configuration
AC_LANG(C++)
CFLAGS=
CPPFLAGS=
CXXFLAGS="-g3"
LDFLAGS=


# Check for some representative header files, and assume their usual
# companions are around as well
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(math.h sys/time.h sys/types.h sys/stat.h unistd.h)
ACT_REQUIRE_HEADER(vector,                  [C++ Standard Template Library])
#ACT_REQUIRE_HEADER(hash_map ext/hash_map,   [C++ STL hash container extensions])
ACT_REQUIRE_HEADER(ext/hash_map,   [C++ STL hash container extensions])
ACT_REQUIRE_HEADER(sstream,                 [C++ IOstream library])
ACT_REQUIRE_HEADER(boost/shared_ptr.hpp,    [Boost smart pointer header files])
ACT_REQUIRE_HEADER(boost/multi_array.hpp,   [Boost multi-dim array header files])
ACT_REQUIRE_HEADER(inca/inca-common.h,      [Inca library header files],
                   /home/jediry/Documents/Development/Media/libinca/src)

ACT_REQUIRE_CXX_LIB(inca, [[#include <inca/math.hpp>]], [inca::math::arcsin(0.5);],
                    [Inca runtime library],
                    [/home/jediry/Documents/Development/Media/libinca/src])


#------------------------------------------------------------------------------
# Should we use the X-Window system?
#------------------------------------------------------------------------------
AC_PATH_XTRA
if test -z "$X_DISPLAY_MISSING"; then
    CXXFLAGS="$CXXFLAGS $X_CFLAGS"
    LDFLAGS="$LDFLAGS $X_LIBS"
fi


#------------------------------------------------------------------------------
# Which rendering interfaces should we use?
#------------------------------------------------------------------------------
ACT_WITH_PACKAGE(opengl, yes, OpenGL for 2D & 3D rendering, [
    ACT_REQUIRE_HEADER(GL/gl.h, OpenGL header file)
    ACT_REQUIRE_HEADER(GL/glu.h, OpenGL Utilities header file)
    ACT_REQUIRE_LIB(GL, glEnd, OpenGL library)
    ACT_REQUIRE_LIB(GLU, gluLookAt, OpenGL Utilities library)
])



# Output resulting files
AC_OUTPUT


# Print out a description of our current configuration
echo
echo "    $PACKAGE_NAME v$PACKAGE_VERSION Compile-time Configuration"
echo
echo "    Feature                      Status"
echo "    ---------------------------- --------------------"
echo
echo
echo "    Environment variables:"
echo "    ----------------------"
echo "    CPPFLAGS = $CPPFLAGS"
echo "    CFLAGS   = $CFLAGS"
echo "    CXXFLAGS = $CXXFLAGS"
echo "    LDFLAGS  = $LDFLAGS"
echo "    LIBS     = $LIBS"
echo
echo "Now type 'make' to build $PACKAGE_NAME"

