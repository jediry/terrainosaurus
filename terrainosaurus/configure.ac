# Process this file with GNU autoconf to produce a configure script.


###############################################################################
##
## Build tools configuration
##
###############################################################################

# Autoconf/Automake configuration
AC_PREREQ(2.53)
AC_INIT(terrainosaurus, 0.2.0, rls@tamu.edu)
AC_CONFIG_SRCDIR(src/terrainosaurus-common.h)
AC_CONFIG_AUX_DIR(config)
#AC_CONFIG_MACRO_DIR(./ac_tools)
AC_CONFIG_FILES(Makefile src/Makefile)
AC_CONFIG_HEADERS(src/config-ac.h)
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
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
ACT_REQUIRE_HEADER(hash_map ext/hash_map,   [C++ STL hash container extensions])
ACT_REQUIRE_HEADER(sstream,                 [C++ IOstream library])
ACT_REQUIRE_HEADER(boost/shared_ptr.hpp,    [Boost smart pointer header files])
ACT_REQUIRE_HEADER(boost/multi_array.hpp,   [Boost multi-dim array header files])
ACT_REQUIRE_HEADER(inca-common.h,           [Inca library header files],
                   /home/jediry/Documents/Development/Media/libinca/src)


ACT_REQUIRE_CXX_LIB(inca, [[#include <math/Math.hpp>]], [Inca::Math::DoubleScalar::sqrt(1.2)],
                    [Inca runtime library],
                    [/home/jediry/Documents/Development/Media/libinca/debug/src])


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

