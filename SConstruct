#! /usr/bin/env python
import os
#from AutoConfigurator import AutoConfigurator

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
# EXTRA COMMAND LINE OPTIONS
###################################################################

AddOption('--flavor', dest = 'flavor', default = 'release',
          nargs = 1, type = 'string', action = 'store',
          metavar = 'FLAVOR', help = 'build flavor (one of {debug, release}')


###################################################################
# LOAD THE ENVIRONMENT AND SET UP THE TOOLS
###################################################################

# Get the build flavor from the command line
flavor = GetOption('flavor')
if not (flavor in ['debug', 'release']):
    print("Error: expected 'debug' or 'release', found: " + flavor)
    Exit(1)
print('**** Compiling in ' + flavor + ' mode...')

variantDir = '#/build/' + flavor
terrainosaurusVariantDir = variantDir + '/terrainosaurus'
incaVariantDir = variantDir + '/inca'
antlrVariantDir = variantDir + '/antlr'
config_h = variantDir + '/config-ac.h'

env = Environment(tools = ['default', 'antlr'], toolpath = ['#/external/scons-tools'])
env.VariantDir(variantDir, 'src', duplicate = 1)

# MSVC: Enable exception unwind semantics
env.Append(CCFLAGS = ['/EHsc'])

# MSVC: Compile for multi-threaded [debug] CRT
env.Append(CCFLAGS = ['/MDd'] if flavor == 'debug' else ['/MD'])

# Define _DEBUG or NDEBUG based on build flavor
env.Append(CCFLAGS = ['/D_DEBUG'] if flavor == 'debug' else ['/DNDEBUG'])

# MSVC: Geerate a PDB
env.Append(LINKFLAGS = ['/DEBUG'])

# MSVC: Enable optimizations in release mode
if flavor == 'release':
    env.Append(CCFLAGS = ['/Ox', '/GL'])
    env.Append(LINKFLAGS = ['/LTCG'])
    env.Append(ARFLAGS = ['/LTCG'])

# MSVC: Compiler warning level
env.Append(CCFLAGS = ['/W1'])

# MSVC: Enable extra debugging checks in debug mode
#if flavor == 'debug':
#    env.Append(CCFLAGS = ['/sdl'])

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

# if not env.GetOption('clean'):

    ## Examine the system configuration
#     conf = Configure(env)
# 
#     autoConf = AutoConfigurator(conf)
# 
#     autoConf.RequireHeader('inca/inca-common.h', language = 'C++')
#     autoConf.RequireLib('inca')
# 
#     autoConf.RequireLib('GL')
#     autoConf.RequireLib('GLU')
#     autoConf.RequireLib('glut')
#     autoConf.RequireLib('fftw3f')
#     autoConf.RequireLib('antlr')
#     autoConf.RequireLib('freeimageplus')
# 
#     env = conf.Finish()

    # Write out the config.h file
#     autoConf.GenerateConfigHeader(config_h)

#env.VariantDir(variantDir, 'src', duplicate = 0)
env.Clean('.', config_h)


###################################################################
# SCRIPTS FOR BUILDING THE TARGETS
###################################################################

Export('env')
env.SConscript('external/antlr-2.7.7/lib/cpp/src/SConscript', variant_dir = antlrVariantDir, duplicate = 0)
env.SConscript('external/inca/SConscript', variant_dir = incaVariantDir, duplicate = 0)
env.SConscript('src/terrainosaurus/SConscript', variant_dir = terrainosaurusVariantDir, duplicate = 1)


###################################################################
# CONVENIENCE FUNCTIONS TO EMULATE 'make dist' and 'make distclean'
###################################################################

### To make a tarball of your masterpiece, use 'scons dist'
if 'dist' in COMMAND_LINE_TARGETS:

	## The target scons dist requires the python module shutil which is in 2.3
	env.EnsurePythonVersion(2, 3)

	import os
	APPNAME = 'bksys'
	VERSION = os.popen("cat VERSION").read().rstrip()
	FOLDER  = APPNAME+'-'+VERSION
	ARCHIVE = FOLDER+'.tar.bz2'

	## If your app name and version number are defined in 'version.h', use this instead:
	## (contributed by Dennis Schridde devurandom@gmx@net)
	#import re
	#INFO = dict( re.findall( '(?m)^#define\s+(\w+)\s+(.*)(?<=\S)', open(r"version.h","rb").read() ) )
	#APPNAME = INFO['APPNAME']
	#VERSION = INFO['VERSION']

	import shutil
	import glob

	## check if the temporary directory already exists
	if os.path.isdir(FOLDER):
		shutil.rmtree(FOLDER)
	if os.path.isfile(ARCHIVE):
		os.remove(ARCHIVE)

	## create a temporary directory
	startdir = os.getcwd()
	shutil.copytree(startdir, FOLDER)

	## remove our object files first
	os.popen("find "+FOLDER+" -name \"*cache*\" | xargs rm -rf")
	os.popen("find "+FOLDER+" -name \"*.pyc\" | xargs rm -f")
	#os.popen("pushd %s && scons -c " % FOLDER) # TODO

	## CVS cleanup
	os.popen("find "+FOLDER+" -name \"CVS\" | xargs rm -rf")
	os.popen("find "+FOLDER+" -name \".cvsignore\" | xargs rm -rf")

	## Subversion cleanup
	os.popen("find %s -name .svn -type d | xargs rm -rf" % FOLDER)

	## GNU Arch cleanup
	os.popen("find "+FOLDER+" -name \"{arch}\" | xargs rm -rf")
	os.popen("find "+FOLDER+" -name \".arch-i*\" | xargs rm -rf")

	## Create the tarball (coloured output)
	print("\033[92m"+"Writing archive "+ARCHIVE+"\033[0m")
	os.popen("tar cjf "+ARCHIVE+" "+FOLDER)

	## Remove the temporary directory
	if os.path.isdir(FOLDER):
		shutil.rmtree(FOLDER)

	env.Default(None)
	env.Exit(0)


### Emulate "make distclean"
if 'distclean' in COMMAND_LINE_TARGETS:
	## Remove the cache directory
	import os, shutil
	if os.path.isdir(env['CACHEDIR']):
		shutil.rmtree(env['CACHEDIR'])
	os.popen("find . -name \"*.pyc\" | xargs rm -rf")

	env.Default(None)
	env.Exit(0)

