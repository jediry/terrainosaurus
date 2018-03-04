#! /usr/bin/env python

"""
help       -> scons -h
compile    -> scons
clean      -> scons -c
install    -> scons install
uninstall  -> scons -c install
configure  -> scons configure prefix=/tmp/ita debug=full extraincludes=/usr/local/include:/tmp/include prefix=/usr/local

Run from a subdirectory -> scons -u
The variables are saved automatically after the first run (look at cache/kde.cache.py, ..)
"""


###################################################################
# Create and configure the Environment
###################################################################

env = Environment(tools = ['default', 'antlr', 'build.flavor'], toolpath = ['#/external/scons-tools'])

flavor = GetOption('flavor')

variantDir = '#/build/' + flavor
terrainosaurusVariantDir = variantDir + '/terrainosaurus'
incaVariantDir = variantDir + '/inca'
antlrVariantDir = variantDir + '/antlr'


###################################################################
# Locate and install build-time dependencies
###################################################################

env.AppendUnique(NUGETROOT = 'D:/NuGetRoot')

env.Append(CPPPATH = ['#/src/', variantDir])
env.Append(CPPPATH = ['$NUGETROOT/boost.1.66.0.0/lib/native/include'])
#env.Append(LIBPATH = ['$NUGETROOT/boost.1.66.0.0/lib/native/include'])
env.Append(CPPPATH = ['$NUGETROOT/nupengl.core.0.1.0.1/build/native/include'])
env.Append(LIBPATH = ['$NUGETROOT/nupengl.core.0.1.0.1/build/native/lib/x64'])
#env.Append(CPPPATH = ['$NUGETROOT/native.freeimage.3.17.0/build/native/include'])
#env.Append(LIBPATH = ['$NUGETROOT/native.freeimage.3.17.0/build/native/lib/Win32'])
env.Append(CPPPATH = ['$NUGETROOT/libfftw.3.3.4/build/native/include'])
env.Append(LIBPATH = ['$NUGETROOT/libfftw.3.3.4/build/native/lib/x64'])
env.Append(CPPPATH = ['#/external/FreeImage/dist/x64'])
env.Append(LIBPATH = ['#/external/FreeImage/dist/x64'])
env.Append(CPPPATH = ['#/external/FreeImage/Wrapper/FreeImagePlus/dist/x64'])
env.Append(LIBPATH = ['#/external/FreeImage/Wrapper/FreeImagePlus/dist/x64'])
env.Append(CPPPATH = ['#/external/antlr-2.7.7/lib/cpp'])
env.Append(LIBPATH = [antlrVariantDir])
env.Append(CPPPATH = ['#/external/inca/src/', '#/external/inca/build/'])
env.Append(LIBPATH = [incaVariantDir + '/src/inca'])

env.Install(terrainosaurusVariantDir, '$NUGETROOT/nupengl.core.redist.0.1.0.1/build/native/bin/x64/freeglut.dll')
env.Install(terrainosaurusVariantDir, '$NUGETROOT/libfftw.redist.3.3.4/build/native/bin/x64/libfftw3f-3.dll')
env.Install(terrainosaurusVariantDir, '#/external/FreeImage/Dist/x64/freeimage.dll')
env.Install(terrainosaurusVariantDir, '#/external/FreeImage/Wrapper/FreeImagePlus/Dist/x64/freeimageplus.dll')


###################################################################
# Build it!
###################################################################

Export('env')
env.SConscript('external/antlr-2.7.7/lib/cpp/src/SConscript', variant_dir = antlrVariantDir, duplicate = 0)
env.SConscript('external/inca/SConscript', variant_dir = incaVariantDir, duplicate = 0)
env.SConscript('src/terrainosaurus/SConscript', variant_dir = terrainosaurusVariantDir, duplicate = 1)
