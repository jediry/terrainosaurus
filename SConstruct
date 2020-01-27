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

DefaultEnvironment(tools = [])
env = Environment(tools = ['default', 'build.flavor', 'c++17', 'antlr4'], toolpath = ['#/external/scons-tools'])

flavor = GetOption('flavor')

variantDir = '#/build/' + flavor
terrainosaurusVariantDir = variantDir + '/terrainosaurus'
incaVariantDir = variantDir + '/inca'

# Here's where VCPkg is located
env['VCPKGROOT'] = '#/external/vcpkg'


###################################################################
# Locate and install build-time dependencies
###################################################################

env.VCPkg('freeimage')
env.VCPkg('antlr4')

env.Append(CPPPATH = ['#/external/inca/src/', '#/src', variantDir])
env.Append(LIBPATH = [incaVariantDir + '/src/inca'])


###################################################################
# Build it!
###################################################################

Export('env')
env.SConscript('external/inca/SConscript', variant_dir = incaVariantDir, duplicate = 0)
env.SConscript('src/terrainosaurus/SConscript', variant_dir = terrainosaurusVariantDir, duplicate = 0)
