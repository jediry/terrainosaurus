#! /usr/bin/env python
import os
from AutoConfigurator import AutoConfigurator

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
# LOAD THE ENVIRONMENT AND SET UP THE TOOLS
###################################################################

buildDir = 'build'
config_h = buildDir + '/config-ac.h'

env = Environment(tools = ['default', 'antlr'], toolpath = ['./tools'])
env.BuildDir(buildDir, 'src', duplicate = 1)
if not os.path.isdir(buildDir):
    os.makedirs(buildDir)

# Hacky, icky way to find libinca headers
env.Append(CPPPATH = ['#/../libinca/src/', '#/../libinca/build/'])
env.Append(LIBPATH = ['#/../libinca/build/'])

if not env.GetOption('clean'):

    ## Examine the system configuration
    conf = Configure(env)

    autoConf = AutoConfigurator(conf)

    autoConf.RequireHeader('inca/inca-common.h', language = 'C++')
    autoConf.RequireLib('inca')

    autoConf.RequireLib('GL')
    autoConf.RequireLib('GLU')
    autoConf.RequireLib('glut')
    autoConf.RequireLib('fftw3f')
    autoConf.RequireLib('antlr')
    autoConf.RequireLib('freeimageplus')

    env = conf.Finish()

    # Write out the config.h file
    autoConf.GenerateConfigHeader(config_h)

env.BuildDir(buildDir, 'src', duplicate = 0)
env.Clean('.', config_h)


###################################################################
# SCRIPTS FOR BUILDING THE TARGETS
###################################################################

Export('env')
env.SConscript('src/SConscript', build_dir = buildDir)


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
	print "\033[92m"+"Writing archive "+ARCHIVE+"\033[0m"
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

