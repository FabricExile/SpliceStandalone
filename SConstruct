import os, platform
import shutil

spliceEnv = Environment()

def RemoveFolderCmd(target, source, env):
  if os.path.exists(source[0].abspath):
    shutil.rmtree(source[0].abspath)

# define the clean target
if 'clean' in COMMAND_LINE_TARGETS:
  cleanBuild = spliceEnv.Command( spliceEnv.File('cleaning build folder'), spliceEnv.Dir('.build'), RemoveFolderCmd )
  cleanStage = spliceEnv.Command( spliceEnv.File('cleaning stage folder'), spliceEnv.Dir('.stage'), RemoveFolderCmd )
  spliceEnv.Alias('clean', [cleanBuild, cleanStage])
  Return()

# check environment variables
for var in ['FABRIC_DIR', 'FABRIC_SPLICE_VERSION', 'FABRIC_BUILD_OS', 'FABRIC_BUILD_ARCH', 'FABRIC_BUILD_TYPE', 'BOOST_DIR', 'QT_INCLUDE_DIR', 'QT_LIB_DIR']:
  if not os.environ.has_key(var):
    print 'The environment variable %s is not defined.' % var
    exit(0)
  if var.lower().endswith('_dir'):
    if not os.path.exists(os.environ[var]):
      print 'The path for environment variable %s does not exist.' % var
      exit(0)

spliceEnv = Environment()

if not os.path.exists(spliceEnv.Dir('.stage').abspath):
  os.makedirs(spliceEnv.Dir('.stage').abspath)

# determine if we have the SpliceAPI two levels up
spliceApiDir = spliceEnv.Dir('..').Dir('..').Dir('SpliceAPI')

# try to get the Splice API from there
if os.path.exists(spliceApiDir.abspath):

  (spliceAPIAlias, spliceAPIFiles) = SConscript(
    dirs = [spliceApiDir],
    exports = {
      'parentEnv': spliceEnv,
      'FABRIC_DIR': os.environ['FABRIC_DIR'],
      'FABRIC_SPLICE_VERSION': os.environ['FABRIC_SPLICE_VERSION'],
      'FABRIC_BUILD_TYPE': os.environ['FABRIC_BUILD_TYPE'],
      'FABRIC_BUILD_OS': os.environ['FABRIC_BUILD_OS'],
      'FABRIC_BUILD_DIST': os.environ['FABRIC_BUILD_DIST'],
      'FABRIC_BUILD_ARCH': os.environ['FABRIC_BUILD_ARCH'],
      'STAGE_DIR': spliceEnv.Dir('.build').Dir('SpliceAPI').Dir('.stage'),
      'BOOST_DIR': os.environ['BOOST_DIR']
    },
    variant_dir = spliceEnv.Dir('.build').Dir('SpliceAPI')
  )
  
  spliceApiDir = spliceEnv.Dir('.build').Dir('SpliceAPI').Dir('.stage').abspath
  
else:

  print 'The folder "'+spliceApiDir.abspath+'" does not exist. Please see the README.md for build instructions.'
  exit(0)

(spliceAlias, spliceFiles) = SConscript(
  os.path.join('SConscript'),
  exports = {
    'parentEnv': spliceEnv,
    'FABRIC_DIR': os.environ['FABRIC_DIR'],
    'FABRIC_SPLICE_VERSION': os.environ['FABRIC_SPLICE_VERSION'],
    'FABRIC_BUILD_TYPE': os.environ['FABRIC_BUILD_TYPE'],
    'FABRIC_BUILD_OS': os.environ['FABRIC_BUILD_OS'],
    'FABRIC_BUILD_ARCH': os.environ['FABRIC_BUILD_ARCH'],
    'FABRIC_BUILD_DIST': os.environ['FABRIC_BUILD_DIST'],
    'STAGE_DIR': spliceEnv.Dir('.stage').Dir('SpliceIntegrations').Dir('SpliceStandalone'+os.environ['FABRIC_SPLICE_VERSION']),
    'BOOST_DIR': os.environ['BOOST_DIR'],
    'QT_INCLUDE_DIR': os.environ['QT_INCLUDE_DIR'],
    'QT_LIB_DIR': os.environ['QT_LIB_DIR'],
    'QT_BINPATH': os.environ.get('QT_BINPATH', None)
  },
  variant_dir = spliceEnv.Dir('.build')
)

allAliases = [spliceAlias]
spliceEnv.Alias('all', allAliases)
spliceEnv.Default(allAliases)
